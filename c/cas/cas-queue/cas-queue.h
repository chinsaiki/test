/**
 *  cas-queue.h
 *  
 *  提供简单低时延的 基于 CAS 操作的队列
 *  
 *  荣涛  2021年1月13日
 *  荣涛  2021年1月14日    添加枚举类型，封装简单接口，但是封装后时延显著增高，见 test-3.c
 *                      这套接口是按照 多入多出多消息传递 设计的。
 *  
 */
#ifndef __CAS_QUEUE_H
#define __CAS_QUEUE_H 1

#ifndef __x86_64__
# error "Not support Your Arch, just support x86-64"
#endif

#include <errno.h>
#include <assert.h>
#include <stdbool.h>

/**
 *  获取tick
 */
#ifndef RDTSC
#define RDTSC() ({\
    register uint32_t a,d; \
    __asm__ __volatile__( "rdtsc" : "=a"(a), "=d"(d)); \
    (((uint64_t)a)+(((uint64_t)d)<<32)); \
    })
#endif
    
/**
 *  cas，默认 smp 架构
 */
#ifndef CAS
#define CAS(ptr, val_old, val_new) ({ \
    char ret; \
    __asm__ __volatile__("lock; "\
        "cmpxchgl %2,%0; setz %1"\
        : "+m"(*ptr), "=q"(ret)\
        : "r"(val_new),"a"(val_old)\
        : "memory"); \
    ret;})
#endif

#ifndef UNUSED
#define UNUSED __attribute__((unused))
#endif
#ifndef likely
#define likely(exp) __builtin_expect(!!exp, 1)
#endif
#ifndef unlikely
#define unlikely(exp) __builtin_expect(!!exp, 0)
#endif

/**
 *  队列状态
 */
typedef enum  {
    READY_TO_CTOR = 1,  /* 准备填入参数 */
    CTORING,            /* 正在填入参数 */
    DONE_TO_CTOR,       /* 填写完参数 */
    READY_TO_ENQUEUE,   /* 准备入队 */
    ENQUEUING,          /* 正在入队 */
    READY_TO_DEQUEUE,   /* 准备出队 */
    DEQUEUING,          /* 正在出队 */
}QUEUE_STATUS;


typedef unsigned long bits_set;
#define BITS_SET_INITIALIZER 0UL

#define BITS_SETSIZE                    64
#define BITS_SET(bit, p_bits_set)       (*(p_bits_set) |= 1UL<<bit)
#define BITS_CLR(bit, p_bits_set)       (*(p_bits_set) &= ~(1UL<<bit))
#define BITS_ISSET(bit, p_bits_set)     (*(p_bits_set) & (1UL<<bit))
#define BITS_ZERO(p_bits_set)           (*(p_bits_set) = 0UL)


/**
 *  快速队列结构体
 */
typedef struct fast_queue_s {
    unsigned long que_id;           //QUEUE_STATUS
    int bitmap_maxbits;             //bitmap_active 中的最大 bit 对应的十进制
    bits_set __attribute__((aligned(64))) bitmap_active;        //标记需要发送的消息 bits
    void    *data[BITS_SETSIZE];    //传递的指针
#define FAST_QUEUE_INITIALIZER  {READY_TO_ENQUEUE, -1, BITS_SET_INITIALIZER, {NULL}}
}__attribute__((aligned(64))) fast_queue_t;



static inline int UNUSED fast_queue_init(fast_queue_t *queue) {

    int i;
    if(unlikely(!queue)) {
        fprintf(stderr, "NULL pointer error\n");
        return -EINVAL;
    }

    memset(queue, 0, sizeof(fast_queue_t));
    queue->que_id = READY_TO_CTOR;
    queue->bitmap_maxbits = -1;

    return 0;
}

static inline fast_queue_t *UNUSED fast_queue_create()
{
    fast_queue_t* queue = malloc(sizeof(fast_queue_t));
    assert(queue);

    fast_queue_init(queue);

    return queue;
}

static inline int UNUSED fast_queue_destroy(fast_queue_t *queue)
{
    if(unlikely(!queue)) {
        fprintf(stderr, "NULL pointer error\n");
        return -EINVAL;
    }
    free(queue);
    
    return 0;
}

static inline int fast_queue_can_add(fast_queue_t *queue) {
    return CAS(&queue->que_id, READY_TO_CTOR, READY_TO_CTOR);
}

    
static inline int UNUSED fast_queue_add_msgs(fast_queue_t *queue, int idx, void *msg) {
#if 0    
    if(unlikely(!queue) || unlikely(!msg)) {
        fprintf(stderr, "NULL pointer error\n");
        return -EINVAL;
    }
    if(unlikely(idx<0) || unlikely(idx>BITS_SETSIZE)) {
        fprintf(stderr, "Message index must between %d and %d\n", 0, BITS_SETSIZE-1);
        return -EINVAL;
    }
#endif    
    while(1) {
        if(CAS(&queue->que_id, READY_TO_CTOR, CTORING)) {
            if(BITS_ISSET(idx, &queue->bitmap_active)) {

                fprintf(stderr, "Already set slot %d\n", idx);

                while(!CAS(&queue->que_id, CTORING, READY_TO_CTOR));
                break;
            }
            if(queue->bitmap_maxbits < idx) {
                queue->bitmap_maxbits = idx;
            }
            
            queue->data[idx] = (void*)msg;
            BITS_SET(idx, &queue->bitmap_active);

            while(!CAS(&queue->que_id, CTORING, READY_TO_CTOR));
            break;
        }
    }
    return 0;
}

static inline int UNUSED fast_queue_add_msgs_done(fast_queue_t *queue) {
    while(!CAS(&queue->que_id, READY_TO_CTOR, READY_TO_ENQUEUE));
}

static inline int UNUSED fast_queue_send(fast_queue_t *queue) {
    while(!CAS(&queue->que_id, READY_TO_ENQUEUE, READY_TO_DEQUEUE));
}

static inline int UNUSED fast_queue_recv(fast_queue_t *queue, int (*msg_handler)(void *msg)) {
    
#if 0    
    if(unlikely(!queue)) {
        fprintf(stderr, "NULL pointer error\n");
        return -EINVAL;
    }
#endif    
    int i = 0, nr_msg = 0;
    if(CAS(&queue->que_id, READY_TO_DEQUEUE, DEQUEUING)) {
        for(i=0; i<= queue->bitmap_maxbits; i++) {
            if(BITS_ISSET(i, &queue->bitmap_active)) {
                msg_handler(queue->data[i]);
                BITS_CLR(i, &queue->bitmap_active);
                nr_msg++;
            }
        }
        queue->bitmap_maxbits = -1;
        CAS(&queue->que_id, DEQUEUING, READY_TO_CTOR);            
    }
    return nr_msg;
}


#endif /*<__CAS_QUEUE_H>*/
