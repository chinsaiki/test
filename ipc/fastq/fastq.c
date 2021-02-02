/**********************************************************************************************************************\
*  文件： fastq.c
*  介绍： 低时延队列
*  作者： 荣涛
*  日期：
*       2021年1月25日    创建与开发轮询功能
*       2021年1月27日 添加 通知+轮询 功能接口，消除零消息时的 CPU100% 问题
*       2021年1月28日 调整代码格式，添加必要的注释
*       2021年2月1日 添加多入单出队列功能 ： 采用 epoll 实现
\**********************************************************************************************************************/
#include <stdint.h>
#include <assert.h>
    
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <syscall.h>
#include <sys/types.h>
#include <sys/eventfd.h> //eventfd
#include <sys/select.h> //FD_SETSIZE
#include <sys/epoll.h>
#include <pthread.h>

#include <stdbool.h>
//#define NDEBUG // (Optional, see assert(3).)
//#include <assert.h>
//#define RB_COMPACT // (Optional, embed color bits in right-child pointers.)
//#include "rb.h"

#include "fastq.h"
#include "atomic.h"
//#include "list.h"



#if (!defined(__i386__) && !defined(__x86_64__))
# error Unsupported CPU
#endif

#ifndef likely
#define likely(x)    __builtin_expect(!!(x), 1)
#endif

#ifndef unlikely
#define unlikely(x)  __builtin_expect(!!(x), 0)
#endif

#ifndef cachelinealigned
#define cachelinealigned __attribute__((aligned(64)))
#endif

//#define FASTQ_DEBUG
#ifdef FASTQ_DEBUG
#define LOG_DEBUG(fmt...)  do{printf("\033[33m[%s:%d]", __func__, __LINE__);printf(fmt);printf("\033[m");}while(0)
#else
#define LOG_DEBUG(fmt...) 
#endif

// FastQRing
struct FastQRing {
    unsigned long src;  //是 1- FASTQ_ID_MAX 的任意值
    unsigned long dst;  //是 1- FASTQ_ID_MAX 的任意值 二者不能重复
    
    unsigned int _size;
    size_t _msg_size;
    char _pad1[64]; //强制对齐，省的 cacheline 64 字节 的限制，下同
    volatile unsigned int _head;
    char _pad2[64];    
    volatile unsigned int _tail;
    char _pad3[64];    
    int _evt_fd; //队列eventfd通知
    char _ring_data[]; //保存实际对象
};


// 从 event fd 查找 ring 的最快方法
static  struct {
    struct FastQRing *_ring;
} _evtfd_to_ring[FD_SETSIZE] = {NULL};


//从 源module ID 和 目的module ID 到 _ring 最快的方法
struct FastQModule {
    bool already_register;
    int epfd; //epoll fd
    unsigned long module_id; //是 1- FASTQ_ID_MAX 的任意值
    unsigned int ring_size; //队列大小，ring 节点数
    unsigned int msg_size; //消息大小， ring 节点大小
    struct FastQRing **_ring;
};
static struct FastQModule *_AllModulesRings = NULL;

static  __attribute__((constructor(101))) __FastQInitCtor () {
    int i, j;
    
    LOG_DEBUG("Init _module_src_dst_to_ring.\n");
    _AllModulesRings = FastQMalloc(sizeof(struct FastQModule)*(FASTQ_ID_MAX+1));
    for(i=0; i<=FASTQ_ID_MAX; i++) {
        _AllModulesRings[i].already_register = false;
        _AllModulesRings[i].module_id = i;
        _AllModulesRings[i].epfd = -1;
        _AllModulesRings[i].ring_size = 0;
        _AllModulesRings[i].msg_size = 0;
        
        _AllModulesRings[i]._ring = FastQMalloc(sizeof(struct FastQRing*)*(FASTQ_ID_MAX+1));
        for(j=0; j<=FASTQ_ID_MAX; j++) { 
            _AllModulesRings[i]._ring[j] = NULL;
        }
    }
}


// 内存屏障
static inline void always_inline mbarrier() { asm volatile("": : :"memory"); }
// This version requires SSE capable CPU.
static inline void always_inline mrwbarrier() { asm volatile("mfence":::"memory"); }
static inline void always_inline mrbarrier()  { asm volatile("lfence":::"memory"); }
static inline void always_inline mwbarrier()  { asm volatile("sfence":::"memory"); }
static inline void always_inline __relax()  { asm volatile ("pause":::"memory"); } 
static inline void always_inline __lock()   { asm volatile ("cli" ::: "memory"); }
static inline void always_inline __unlock() { asm volatile ("sti" ::: "memory"); }


always_inline static unsigned int 
__power_of_2(unsigned int size) {
    unsigned int i;
    for (i=0; (1U << i) < size; i++);
    return 1U << i;
}

always_inline  static struct FastQRing *
__fastq_create_ring(const int epfd, const unsigned long src, const unsigned long dst, 
                      const unsigned int ring_size, const unsigned int msg_size) {
    struct FastQRing *new_ring = FastQMalloc(sizeof(struct FastQRing) + ring_size*(msg_size+sizeof(size_t)));
    assert(new_ring);
    assert(epfd);
    
    LOG_DEBUG("Create ring %ld->%ld.\n", src, dst);
    new_ring->src = src;
    new_ring->dst = dst;
    new_ring->_size = ring_size - 1;
    
    new_ring->_msg_size = msg_size + sizeof(size_t);
    new_ring->_evt_fd = eventfd(0, EFD_CLOEXEC);
    assert(new_ring->_evt_fd); //都TMD没有fd了，你也是厉害
    
    LOG_DEBUG("Create module %ld event fd = %d.\n", dst, new_ring->_evt_fd);
    
    _evtfd_to_ring[new_ring->_evt_fd]._ring = new_ring;

    struct epoll_event event;
    event.data.fd = new_ring->_evt_fd;
    event.events = EPOLLIN; //必须采用水平触发
    epoll_ctl(epfd, EPOLL_CTL_ADD, event.data.fd, &event);
    LOG_DEBUG("Add eventfd %d to epollfd %d.\n", new_ring->_evt_fd, epfd);

    return new_ring;
    
}


always_inline void 
FastQCreateModule(const unsigned long module_id, const unsigned int ring_size, const unsigned int msg_size) {
    assert(module_id <= FASTQ_ID_MAX);
    
    int i, j;
    
    if(_AllModulesRings[module_id].already_register) {
        LOG_DEBUG("Module %ld already register.\n", module_id);
        return false;
    }
    _AllModulesRings[module_id].already_register = true;
    _AllModulesRings[module_id].epfd = epoll_create(1);
    assert(_AllModulesRings[module_id].epfd);
        
    LOG_DEBUG("Create module %ld epoll fd = %d.\n", module_id, _AllModulesRings[module_id].epfd);
    
    _AllModulesRings[module_id].ring_size = __power_of_2(ring_size);
    _AllModulesRings[module_id].msg_size = msg_size;

    for(i=1; i<=FASTQ_ID_MAX && i != module_id; i++) {
        if(!_AllModulesRings[i].already_register) {
            continue;
        }
        _AllModulesRings[module_id]._ring[i] = __fastq_create_ring(_AllModulesRings[module_id].epfd, i, module_id,\
                                                                    __power_of_2(ring_size), msg_size);
        if(!_AllModulesRings[i]._ring[module_id]) {
            _AllModulesRings[i]._ring[module_id] = __fastq_create_ring(_AllModulesRings[i].epfd, module_id, i, \
                                                    _AllModulesRings[module_id].ring_size, _AllModulesRings[i].msg_size);
        }
    }
}


always_inline static bool 
__FastQSend(struct FastQRing *ring, const void *msg, const size_t size) {
    assert(ring);
    assert(size <= (ring->_msg_size - sizeof(size_t)));

    unsigned int h = (ring->_head - 1) & ring->_size;
    unsigned int t = ring->_tail;
    if (t == h) {
        return false;
    }

    LOG_DEBUG("Send %ld->%ld.\n", ring->src, ring->dst);

    char *d = &ring->_ring_data[t*ring->_msg_size];
    
    memcpy(d, &size, sizeof(size));
    LOG_DEBUG("Send >>> memcpy size = %d\n", size);
    memcpy(d + sizeof(size), msg, size);
    LOG_DEBUG("Send >>> memcpy addr = %x\n", *(unsigned long*)(d + sizeof(size)));

    // Barrier is needed to make sure that item is updated 
    // before it's made available to the reader
    mwbarrier();

    ring->_tail = (t + 1) & ring->_size;
    return true;
}

always_inline bool 
FastQSend(unsigned int from, unsigned int to, const void *msg, size_t size) {
    struct FastQRing *ring = _AllModulesRings[to]._ring[from];
    while (!__FastQSend(ring, msg, size)) {__relax();}
    
    eventfd_write(ring->_evt_fd, 1);
    
    LOG_DEBUG("Send done %ld->%ld, event fd = %d, msg = %p.\n", ring->src, ring->dst, ring->_evt_fd, msg);
    return true;
}

always_inline bool 
FastQTrySend(unsigned int from, unsigned int to, const void *msg, size_t size) {
    struct FastQRing *ring = _AllModulesRings[to]._ring[from];
    bool ret = __FastQSend(ring, msg, size);
    if(ret) {
        eventfd_write(ring->_evt_fd, 1);
        LOG_DEBUG("Send done %ld->%ld, event fd = %d.\n", ring->src, ring->dst, ring->_evt_fd);
    }
    return ret;
}

always_inline static bool 
__FastQRecv(struct FastQRing *ring, void *msg, size_t *size) {

    unsigned int t = ring->_tail;
    unsigned int h = ring->_head;
    if (h == t) {
//        LOG_DEBUG("Recv <<< %ld->%ld.\n", ring->src, ring->dst);
        return false;
    }
    
    LOG_DEBUG("Recv <<< %ld->%ld.\n", ring->src, ring->dst);

    char *d = &ring->_ring_data[h*ring->_msg_size];

    size_t recv_size;
    memcpy(&recv_size, d, sizeof(size_t));
    LOG_DEBUG("Recv <<< memcpy recv_size = %d\n", recv_size);
    assert(recv_size <= *size && "buffer too small");
    *size = recv_size;
    LOG_DEBUG("Recv <<< size\n");
    memcpy(msg, d + sizeof(size_t), recv_size);
    LOG_DEBUG("Recv <<< memcpy addr = %x\n", *(unsigned long*)(d + sizeof(size_t)));

    // Barrier is needed to make sure that we finished reading the item
    // before moving the head
    mbarrier();
    LOG_DEBUG("Recv <<< mbarrier\n");

    ring->_head = (h + 1) & ring->_size;
    return true;
}


always_inline  bool 
FastQRecv(unsigned int from, msg_handler_t handler) {

    eventfd_t cnt;
    int nfds;
    struct epoll_event events[8];
    
    unsigned long addr;
    size_t size = sizeof(unsigned long);
    struct FastQRing *ring = NULL;

    while(1) {
        LOG_DEBUG("Recv start >>>>  epoll fd = %d.\n", _AllModulesRings[from].epfd);
        nfds = epoll_wait(_AllModulesRings[from].epfd, events, 8, -1);
        LOG_DEBUG("Recv epoll return epfd = %d.\n", _AllModulesRings[from].epfd);
        
        for(;nfds--;) {
            ring = _evtfd_to_ring[events[nfds].data.fd]._ring;
            eventfd_read(events[nfds].data.fd, &cnt);
//            if(cnt>1) {
//                printf("cnt = =%ld\n", cnt);
//            }
            LOG_DEBUG("Event fd %d read return cnt = %ld.\n", events[nfds].data.fd, cnt);
            for(; cnt--;) {
                LOG_DEBUG("<<< __FastQRecv.\n");
                while (!__FastQRecv(ring, &addr, &size)) {
                    __relax();
                }
                LOG_DEBUG("<<< __FastQRecv addr = %x, size = %ld.\n", addr, size);
                handler(addr, size);
                LOG_DEBUG("<<< __FastQRecv.\n");
                addr = 0;
                size = sizeof(unsigned long);
            }

        }
    }
    return true;
}


always_inline void 
FastQDump(FILE*fp) {
    if(unlikely(!fp) || unlikely(!fp)) {
        fp = stderr;
    }
    
    int i, j;
    
    for(i=1; i<=FASTQ_ID_MAX; i++) {
        if(!_AllModulesRings[i].already_register) {
            continue;
        }
        fprintf(fp, "\n------------------------------------------\n"\
                    "ID: %3i, msgMax %4u, msgSize %4u\n"\
                    "\t from-> to   %10s %10s\n", 
                    i, _AllModulesRings[i].ring_size, _AllModulesRings[i].msg_size, "head", "tail");
        
        for(j=0; j<=FASTQ_ID_MAX; j++) { 
            if(_AllModulesRings[i]._ring[j]) {
                fprintf(fp, "\t %4i->%-4i  %10u %10u\n", \
                                j, i, _AllModulesRings[i]._ring[j]->_head, _AllModulesRings[i]._ring[j]->_tail);
            }
        }
    }
}

