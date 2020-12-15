/************************************************************************************************************\
 *
 *  hpmalloc - 高性能内存分配器
 *
 *  作者: 荣涛
 *  日期:   2020年12月
 *
 ************************************************************************************************************
 *
 *  高性能内存分配接口 
 *      -------------------------------------------------------------------
 *      void *hpmalloc(size_t size, module_id_t module_id);
 *      void *hpcalloc(size_t number, size_t size, module_id_t module_id);
 *      void *hprealloc(void *ptr, size_t size, module_id_t module_id);
 *      void hpfree(void *ptr);
 *      char *hpstrdup(const char *s, module_id_t module_id);
 *
 *  高性能检测DEBUG类接口
 *      -------------------------------------------------------------------
 *      int hpmemcheck(void *addr);
 *
 *  高性能统计类接口
 *      -------------------------------------------------------------------
 *      size_t hpmalloc_used_memory(void);
 *      size_t hpmalloc_module_used_memory(module_id_t module_id);
 *      size_t hpmsize(void *const ptr);
 *
 *  
 *  统计类接口
 *      -------------------------------------------------------------------
 *      size_t hpmalloc_get_memory_size(void);
 *      size_t hpmalloc_get_private_dirty(long pid);
 *      size_t hpmalloc_get_shared_dirty(long pid);size_t hpmalloc_get_rss(void);
 *      int hpmalloc_get_allocator_info(size_t *allocated, size_t *active, size_t *resident);
 *      size_t hpmalloc_get_rss(void);
 *  
\************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>

#include "hpcommon.h"
#include "hpmalloc.h"
#include "hplist.h"


/* magic 数，用于特殊的标记 */
#define HPMEM_MAGIC 0xcbaaabcd

 

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*\
 *  调试信息 结构的开始
\*--------------------------------------------------------------------------------------------------------------------*/
#ifdef HPMEM_DEBUG
#define DEBUG_CHUNK_LIST_PREFIX struct list_head list;
#define DEBUG_CHUNK_LIST_PREFIX_LEN sizeof(struct list_head)

static LIST_HEAD(chunk_list_head);
static pthread_rwlock_t chunk_list_lock = PTHREAD_RWLOCK_INITIALIZER;

#define INSERT_CHUNK_TO_LIST(pchunk) \
    pthread_rwlock_wrlock(&chunk_list_lock); \
    list_add_tail(&pchunk->list, &chunk_list_head);\
    pthread_rwlock_unlock(&chunk_list_lock); 

#define DELETE_CHUNK_FROM_LIST(pchunk) \
    pthread_rwlock_wrlock(&chunk_list_lock); \
    list_del(&pchunk->list);\
    pthread_rwlock_unlock(&chunk_list_lock); 


/*--------------------------------------------------------------------------------------------------------------------*/
#else

#define DEBUG_CHUNK_LIST_PREFIX
#define DEBUG_CHUNK_LIST_PREFIX_LEN (0)
#define INSERT_CHUNK_TO_LIST(pchunk)
#define DELETE_CHUNK_FROM_LIST(pchunk)


#endif


/*--------------------------------------------------------------------------------------------------------------------*\
 *  调试信息 结构的结束
\*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*\
 *  高性能内存分配器的开始
\*--------------------------------------------------------------------------------------------------------------------*/

/* 实际分配的内存块 */
struct hpmem_chunk;

/* 模块调试信息 */
struct module_debug_info_s{
    size_t used_memory;         //该模块使用的内存总数
};

/* 最大支持256个内存块 */
static struct module_debug_info_s module_debug_info[256] = {
    [0 ... 255] = {0, },
};


/*--------------------------------------------------------------------------------------------------------------------*\
 *  使用 前缀 和 后缀 检测内存越界问题
\*--------------------------------------------------------------------------------------------------------------------*/
/* 内存块的前缀，如果 DEBUG 功能未开启，该结构在 #else 中定义 */
#define HMPEM_CHUNK_PREFIX  \
    module_id_t module_id; \
    unsigned long  size; \
    DEBUG_CHUNK_LIST_PREFIX \
    unsigned long magic;


#define HPMEM_CHUNK_MODULE_ID(pchunk)   (pchunk->module_id)
#define HPMEM_CHUNK_SIZE(pchunk)        (pchunk->size)
#define HPMEM_CHUNK_MAGIC_HEAD(pchunk)  (pchunk->magic)


/* 前缀对应长度 */
#define HPMEM_CHUNK_PREFIX_LEN (sizeof(module_id_t)+sizeof(unsigned long)+\
                                    sizeof(unsigned long )+DEBUG_CHUNK_LIST_PREFIX_LEN)

/* 前缀 fill */
#define HMPEM_CHUNK_SET_PREFIX(pchunk, id, size)\
    HPMEM_CHUNK_MODULE_ID(pchunk) = id; \
    HPMEM_CHUNK_SIZE(pchunk) = size; \
    HPMEM_CHUNK_MAGIC_HEAD(pchunk) = HPMEM_MAGIC


/* DEBUG 模式下，后缀为一个 magic number */
#define HPMEM_CHUNK_SUBFIX_LEN  sizeof(unsigned long) /* magic number */

#define HPMEM_CHUNK_MAGIC_TAIL(pchunk)  *(unsigned long*)((char*)pchunk+ \
                                                HPMEM_CHUNK_PREFIX_LEN+HPMEM_CHUNK_SIZE(pchunk))

/* 设置后缀（在前缀 fill 之后才可进行的操作） */
#define HMPEM_CHUNK_SET_SUBFIX(pchunk) \
    HPMEM_CHUNK_MAGIC_TAIL(pchunk) = HPMEM_MAGIC
    
/* 记录各模块的内存只用情况 */
#define module_used_mem_incr(mid, size) atomic_incr(module_debug_info[mid].used_memory,size)
#define module_used_mem_decr(mid, size) atomic_decr(module_debug_info[mid].used_memory,size)
#define module_used_mem_get(mid, um) atomic_get(module_debug_info[mid].used_memory,um)

/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*\
 *  管理的 chunk
\*--------------------------------------------------------------------------------------------------------------------*/

/* 用户申请的内存块 chunk */
struct hpmem_chunk{
    HMPEM_CHUNK_PREFIX
    char addr[0];
};

/* 总内存大小 */
static size_t total_used_memory = 0;

/* 错误码对应的可读字符串 */
const struct {
    int e_number;
    char *e_string;
} hpmem_strerror[HPMEM_EMAX] = {
    [HPMEM_EOK]     = {HPMEM_EOK,       "mem is fine"},/* OK */
    [HPMEM_EPARAM]  = {HPMEM_EPARAM,    "invalid parameter",},/* 不可用的参数 */
    [HPMEM_EHEAD]   = {HPMEM_EHEAD,     "memory before the block was clobbered",},/* 内存前端越界 */
    [HPMEM_ETAIL]   = {HPMEM_ETAIL,     "memory after the block was clobbered"},/* 内存尾部越界 */
    [HPMEM_ENOTSUPPORT]   = {HPMEM_ENOTSUPPORT,     "not support"},/* 不支持 */
};

/**
 *  hpmalloc_strerror 分配器错误字符串
 *
 *  errNo 错误码
 */
const char*const hpmalloc_strerror(hpmem_errno_t errNo)
{
    if(errNo < HPMEM_EMAX && errNo >= 0)
        return hpmem_strerror[errNo].e_string;
}

/* This function provide us access to the original libc free(). This is useful
 * for instance to free results obtained by backtrace_symbols(). We need
 * to define this function before including hpmalloc.h that may shadow the
 * free implementation if we use jemalloc or another non standard allocator. */
void hplibc_free(void *ptr) 
{
    free(ptr);
}




/*--------------------------------------------------------------------------------------------------------------------*\
 *  DEBUG调试代码 开始
\*--------------------------------------------------------------------------------------------------------------------*/
static int hpmem_chunk_is_ok_abort(struct hpmem_chunk *chunk)
{
    if(unlikely(HPMEM_CHUNK_MAGIC_HEAD(chunk) != HPMEM_MAGIC)) {
        fprintf(stderr, "\033[1;5;31m[ID:%ld, %d Bytes] %s, addr %p\033[m\n", 
                                                          HPMEM_CHUNK_MODULE_ID(chunk), 
                                                          HPMEM_CHUNK_SIZE(chunk), 
                                                          hpmem_strerror[HPMEM_EHEAD].e_string,
                                                          chunk->addr);
        goto mem_dump_error;
    }
    if(unlikely(HPMEM_CHUNK_MAGIC_TAIL(chunk) != HPMEM_MAGIC)) {
        fprintf(stderr, "\033[1;5;31m[ID:%ld, %d Bytes] %s, addr %p\033[m\n", 
                                                          HPMEM_CHUNK_MODULE_ID(chunk), 
                                                          HPMEM_CHUNK_SIZE(chunk), 
                                                          hpmem_strerror[HPMEM_ETAIL].e_string,
                                                          chunk->addr);
        goto mem_dump_error;
    }
    if(unlikely(HPMEM_CHUNK_MODULE_ID(chunk) <= 0)) {
        fprintf(stderr, "\033[1;5;31m[%d Bytes] %s, addr %p\033[m\n", 
                                                          HPMEM_CHUNK_SIZE(chunk), 
                                                          hpmem_strerror[HPMEM_EPARAM].e_string,
                                                          chunk->addr);
        goto mem_dump_error;
    }
    if(unlikely(HPMEM_CHUNK_SIZE(chunk) <= 0)) {
        fprintf(stderr, "\033[1;5;31m[ID:%ld] %s, addr %p\033[m\n", 
                                                          HPMEM_CHUNK_MODULE_ID(chunk),
                                                          hpmem_strerror[HPMEM_EPARAM].e_string,
                                                          chunk->addr);
        goto mem_dump_error;
    }
    return 1;
    
    
mem_dump_error:
    
    fprintf(stderr, "hpmalloc: memory used ERROR.\n");
    fflush(stderr);
    abort();
}

#ifdef HPMEM_DEBUG

static pthread_t debug_check_chunk_list_task;

static void *debug_check_chunk_list_task_routine(void*arg){
    struct hpmem_chunk *chunk, *iter_next;
    
    while(1) {
        pthread_rwlock_rdlock(&chunk_list_lock); 
        list_for_each_entry_safe(chunk, iter_next, &chunk_list_head, list) {
            hpmem_chunk_is_ok_abort(chunk);
        }
        pthread_rwlock_unlock(&chunk_list_lock); 
        usleep(10000);
    }
}

static  __attribute__((constructor(101))) void debug_check_chunk_list_init() {
    pthread_create(&debug_check_chunk_list_task, NULL, debug_check_chunk_list_task_routine, NULL);
}
#endif
/*--------------------------------------------------------------------------------------------------------------------*\
 *  DEBUG调试代码 结束
\*--------------------------------------------------------------------------------------------------------------------*/


/* Explicitly override malloc/free etc when using tcmalloc. */
#if defined(USE_TCMALLOC)
 #define __malloc(size) tc_malloc(size)
 #define __calloc(count,size) tc_calloc(count,size)
 #define __realloc(ptr,size) tc_realloc(ptr,size)
 #define __free(ptr) tc_free(ptr)
#elif defined(USE_JEMALLOC)
 #define __malloc(size) je_malloc(size)
 #define __calloc(count,size) je_calloc(count,size)
 #define __realloc(ptr,size) je_realloc(ptr,size)
 #define __free(ptr) je_free(ptr)
 #define __mallocx(size,flags) je_mallocx(size,flags)
 #define __dallocx(ptr,flags) je_dallocx(ptr,flags)
#else
 #define __malloc(size) malloc(size)
 #define __calloc(count,size) calloc(count,size)
 #define __realloc(ptr,size) realloc(ptr,size)
 #define __free(ptr) free(ptr)
#endif

#define update_hpmalloc_stat_alloc(module_id,__n) do { \
    size_t _n = (__n); \
    if (_n&(sizeof(long)-1)) _n += sizeof(long)-(_n&(sizeof(long)-1)); \
    atomic_incr(total_used_memory,__n); \
    module_used_mem_incr(module_id,__n); \
} while(0)

#define update_hpmalloc_stat_free(module_id,__n) do { \
    size_t _n = (__n); \
    if (_n&(sizeof(long)-1)) _n += sizeof(long)-(_n&(sizeof(long)-1)); \
    atomic_decr(total_used_memory,__n); \
    module_used_mem_decr(module_id,__n); \
} while(0)


static void hpmalloc_default_oom(size_t size) 
{
    fprintf(stderr, "hpmalloc: Out of memory trying to allocate %zu bytes\n",
        size);
    fflush(stderr);
    abort();
}

static void (*hpmalloc_oom_handler)(size_t) = hpmalloc_default_oom;


/**
 *  hpmalloc_version 版本信息
 */
const char* hpmalloc_version(void)
{
#ifdef HPMALLOC_LIB
    return HPMALLOC_LIB;
#else
    return "unknown";
#endif
}

/**
 *  hpmalloc_type 分配器类型
 */
HPMALLOC_TYPE hpmalloc_type(void)
{
#if defined(USE_TCMALLOC)
    return HP_TCMALLOC;
#elif defined(USE_JEMALLOC)
    return HP_JEMALLOC;
#else
    return HP_PTMALLOC;
#endif
    
}


static size_t hpmalloc_size(void *const ptr)
{
#if defined(USE_TCMALLOC)
    return tc_malloc_size(ptr);
#elif defined(USE_JEMALLOC)
    return je_malloc_usable_size(ptr);
#else
    return malloc_usable_size(ptr);
#endif
}

/**
 *  hpmsize 申请的内存块大小
 *  
 *  ptr 通过内存分配函数申请的内存地址
 *  
 *  返回      内存大小
 */
size_t hpmsize(void *const ptr)
{
    struct hpmem_chunk *addr = container_of(ptr, struct hpmem_chunk, addr);
    return hpmalloc_size(addr) - HPMEM_CHUNK_PREFIX_LEN - HPMEM_CHUNK_SUBFIX_LEN;
}


/**
 *  hpmalloc 内存申请
 *  
 *  参数
 *  size        内存申请大小 Bytes
 *  module_id   在debug模式下，该字段用于 记录 模块ID 使用的内存大小
 *  
 *  返回值       内存地址
 */
void *hpmalloc(size_t size, module_id_t module_id) 
{
    struct hpmem_chunk *ptr = __malloc(size + HPMEM_CHUNK_PREFIX_LEN + HPMEM_CHUNK_SUBFIX_LEN);

    if (!ptr) hpmalloc_oom_handler(size);

    HMPEM_CHUNK_SET_PREFIX(ptr, module_id, size);   
    HMPEM_CHUNK_SET_SUBFIX(ptr);
    update_hpmalloc_stat_alloc(module_id, hpmalloc_size(ptr));

    INSERT_CHUNK_TO_LIST(ptr);
    
    return ptr->addr;
}

/**
 *  hpcalloc 内存申请
 *  
 *  参数
 *  number      个数
 *  size        内存申请大小 Bytes
 *  module_id   在 debug 模式下，该字段用于 记录 模块ID 使用的内存大小
 *  
 *  返回值       内存空间首地址
 */
void *hpcalloc(size_t number, size_t size, module_id_t module_id) 
{
    struct hpmem_chunk *ptr = __calloc(1, number*size + HPMEM_CHUNK_PREFIX_LEN + HPMEM_CHUNK_SUBFIX_LEN);

    if (!ptr) hpmalloc_oom_handler(size);

    HMPEM_CHUNK_SET_PREFIX(ptr, module_id, size);   
    HMPEM_CHUNK_SET_SUBFIX(ptr);

    update_hpmalloc_stat_alloc(module_id, hpmalloc_size(ptr));
    
    INSERT_CHUNK_TO_LIST(ptr);
    
    return ptr->addr;
}


/**
 *  hprealloc 重申请内存
 *  
 *  参数
 *  ptr         使用 hpmalloc 或 hpcalloc 申请的内存地址
 *  size        内存申请大小 Bytes
 *  module_id   在 debug 模式下，该字段用于 记录 模块ID 使用的内存大小
 *  
 *  返回值       内存空间首地址
 */
void *hprealloc(void *addr, size_t size, module_id_t module_id) 
{
    void *realptr;
    size_t oldsize;
    struct hpmem_chunk *newptr;

    struct hpmem_chunk *ptr = container_of(addr, struct hpmem_chunk, addr);

    if (size == 0 && ptr != NULL) {
        hpfree(addr);
        return NULL;
    }
    if (ptr == NULL) return hpmalloc(size, module_id);
    oldsize = hpmalloc_size(ptr);
    
    newptr = __realloc(ptr,size + HPMEM_CHUNK_PREFIX_LEN + HPMEM_CHUNK_SUBFIX_LEN);
    if (!newptr) hpmalloc_oom_handler(size);

    
    HMPEM_CHUNK_SET_PREFIX(newptr, module_id, size);   
    HMPEM_CHUNK_SET_SUBFIX(newptr);

    update_hpmalloc_stat_free(module_id, oldsize);
    update_hpmalloc_stat_alloc(module_id, hpmalloc_size(newptr));
    
    return newptr->addr;
}

/**
 *  hpfree 内存释放
 *  
 *  参数
 *  ptr         使用 hpmalloc 或 hpcalloc 申请的内存地址
 *  
 *  返回值 无
 */
void hpfree(void *addr) 
{
    if (addr == NULL) return;
    
    struct hpmem_chunk *chunk = container_of(addr, struct hpmem_chunk, addr);
    
    update_hpmalloc_stat_free(chunk->module_id, hpmalloc_size(chunk));

    DELETE_CHUNK_FROM_LIST(chunk);

    hpmem_chunk_is_ok_abort(chunk);
    
    __free(chunk);
}

/**
 *  hpstrdup 复制字符串
 *  
 *  参数
 *  s   源字符串
 *  module_id   在 debug 模式下，该字段用于 记录 模块ID 使用的内存大小
 *  
 *  返回值       复制的字符串的首地址
 */
char *hpstrdup(const char *s, module_id_t module_id) 
{
    size_t l = strlen(s)+1;
    char *p = hpmalloc(l, module_id);

    memcpy(p,s,l);
    return p;
}


int hpmemcheck(void *addr) 
{
    struct hpmem_chunk *ptr = container_of(addr, struct hpmem_chunk, addr);

    if(unlikely(HPMEM_CHUNK_MAGIC_HEAD(ptr) != HPMEM_MAGIC)) return HPMEM_EHEAD;
    if(unlikely(HPMEM_CHUNK_MAGIC_TAIL(ptr) != HPMEM_MAGIC)) return HPMEM_ETAIL;
    if(unlikely(HPMEM_CHUNK_MODULE_ID(ptr) <= 0)) return HPMEM_EPARAM;
    if(unlikely(HPMEM_CHUNK_SIZE(ptr) <= 0)) return HPMEM_EPARAM;

    return HPMEM_EOK;
}

/**
 *  hpmalloc_used_memory 获取使用的内存大小
 *  
 *  返回      申请的内存总大小
 */
size_t hpmalloc_used_memory(void) 
{
    size_t um;
    atomic_get(total_used_memory,um);
    return um;
}

/**
 *  hpmalloc_module_used_memory 获取（模块）使用的内存大小
 *  
 *  module_id 模块ID
 *
 *  返回      申请的内存总大小
 */
size_t hpmalloc_module_used_memory(module_id_t module_id)
{
    size_t um;
    module_used_mem_get(module_id,um);
    return um;
}

/**
 *  设置 Out Of Memory 回调函数
 */
void hpmalloc_set_oom_handler(void (*oom_handler)(size_t)) 
{
    hpmalloc_oom_handler = oom_handler;
}

/**
 *  hpmalloc_get_rss 获取实际使用的物理内存大小（包含共享内存库）
 *
 *   实际使用的物理内存（包含共享库占用的内存）
 *
 * 注: RSS Resident Set Size，该接口不考虑性能，不应在代码中周期调用
 *
 */
size_t hpmalloc_get_rss(void) 
{
    int page = sysconf(_SC_PAGESIZE);
    size_t rss;
    char buf[4096];
    char filename[256];
    int fd, count;
    char *p, *x;

    snprintf(filename,256,"/proc/%d/stat",getpid());
    if ((fd = open(filename,O_RDONLY)) == -1) return 0;
    if (read(fd,buf,4096) <= 0) {
        close(fd);
        return 0;
    }
    close(fd);

    p = buf;
    count = 23; /* RSS is the 24th field in /proc/<pid>/stat */
    while(p && count--) {
        p = strchr(p,' ');
        if (p) p++;
    }
    if (!p) return 0;
    x = strchr(p,' ');
    if (!x) return 0;
    *x = '\0';

    rss = strtoll(p,NULL,10);
    rss *= page;
    return rss;
}

#if defined(USE_TCMALLOC)
/**
 *  hpmalloc_get_allocator_info 获取总内存分配和剩余大小，采用不同的分配器，值会有所差异
 *  
 *  allocated   申请的内存大小
 *  active      激活的内存大小
 *  resident    剩余内存大小
 */
int hpmalloc_get_allocator_info(size_t *allocated, size_t *active, size_t *resident) 
{
    struct mallinfo info = tc_mallinfo();

    *allocated = *active = info.uordblks;
    *resident = info.fordblks;

    return 1;
}
int hpmalloc_set_jemalloc_bg_thread(int enable) 
{
    ((void)(enable));
    return HPMEM_ENOTSUPPORT;
}

int hpmalloc_jemalloc_purge() 
{
    return HPMEM_ENOTSUPPORT;
}



#elif defined(USE_JEMALLOC)
/**
 *  hpmalloc_get_allocator_info 获取总内存分配和剩余大小，采用不同的分配器，值会有所差异
 *  
 *  allocated   申请的内存大小
 *  active      激活的内存大小
 *  resident    剩余内存大小
 */
int hpmalloc_get_allocator_info(size_t *allocated, size_t *active, size_t *resident) 
{
    uint64_t epoch = 1;
    size_t sz;
    *allocated = *resident = *active = 0;
    /* Update the statistics cached by mallctl. */
    sz = sizeof(epoch);
    je_mallctl("epoch", &epoch, &sz, &epoch, sz);
    sz = sizeof(size_t);
    /* Unlike RSS, this does not include RSS from shared libraries and other non
     * heap mappings. */
    je_mallctl("stats.resident", resident, &sz, NULL, 0);
    /* Unlike resident, this doesn't not include the pages jemalloc reserves
     * for re-use (purge will clean that). */
    je_mallctl("stats.active", active, &sz, NULL, 0);
    /* Unlike hpmalloc_used_memory, this matches the stats.resident by taking
     * into account all allocations done by this process (not only hpmalloc). */
    je_mallctl("stats.allocated", allocated, &sz, NULL, 0);
    return 1;
}

/**
 *  hpmalloc_set_jemalloc_bg_thread 设置jemalloc后台的内存分配器管理线程数使能
 *  
 *  enable 为真 使能， 否则，无效。
 */
int hpmalloc_set_jemalloc_bg_thread(int enable) 
{
    /* let jemalloc do purging asynchronously, required when there's no traffic 
     * after flushdb */
    char val = !!enable;
    je_mallctl("background_thread", NULL, 0, &val, 1);
}

/**
 *  hpmalloc_jemalloc_purge 将长时间不用的 page 归还给 系统内核
 */
int hpmalloc_jemalloc_purge() 
{
    /* return all unused (reserved) pages to the OS */
    char tmp[32];
    unsigned narenas = 0;
    size_t sz = sizeof(unsigned);
    if (!je_mallctl("arenas.narenas", &narenas, &sz, NULL, 0)) {
        sprintf(tmp, "arena.%d.purge", narenas);
        if (!je_mallctl(tmp, NULL, 0, NULL, 0))
            return 0;
    }
    return -1;
}

#else
/**
 *  hpmalloc_get_allocator_info 获取总内存分配和剩余大小，采用不同的分配器，值会有所差异
 *  
 *  allocated   申请的内存大小
 *  active      激活的内存大小
 *  resident    剩余内存大小
 */
int hpmalloc_get_allocator_info(size_t *allocated, size_t *active, size_t *resident) 
{
    struct mallinfo info = mallinfo();

    *allocated = *active = info.uordblks;
    *resident = info.fordblks;

    return 1;
}

int hpmalloc_set_jemalloc_bg_thread(int enable) 
{
    ((void)(enable));
    return HPMEM_ENOTSUPPORT;
}

int hpmalloc_jemalloc_purge() 
{
    return HPMEM_ENOTSUPPORT;
    return 0;
}
#endif


/* Get the sum of the specified field (converted form kb to bytes) in
 * /proc/self/smaps. The field must be specified with trailing ":" as it
 * apperas in the smaps output.
 *
 * If a pid is specified, the information is extracted for such a pid,
 * otherwise if pid is -1 the information is reported is about the
 * current process.
 *
 * Example: hpmalloc_get_smap_bytes_by_field("Rss:",-1);
 */
static size_t hpmalloc_get_smap_bytes_by_field(char *field, long pid) 
{
    char line[1024];
    size_t bytes = 0;
    int flen = strlen(field);
    FILE *fp;

    if (pid == -1) {
        fp = fopen("/proc/self/smaps","r");
    } else {
        char filename[128];
        snprintf(filename,sizeof(filename),"/proc/%ld/smaps",pid);
        fp = fopen(filename,"r");
    }

    if (!fp) return 0;
    while(fgets(line,sizeof(line),fp) != NULL) {
        if (strncmp(line,field,flen) == 0) {
            char *p = strchr(line,'k');
            if (p) {
                *p = '\0';
                bytes += strtol(line+flen,NULL,10) * 1024;
            }
        }
    }
    fclose(fp);
    return bytes;
}

size_t hpmalloc_get_private_dirty(long pid) 
{
    return hpmalloc_get_smap_bytes_by_field("Private_Dirty:",pid);
}
size_t hpmalloc_get_shared_dirty(long pid)
{
    return hpmalloc_get_smap_bytes_by_field("Shared_Dirty:",pid);
}

/* Returns the size of physical memory (RAM) in bytes.
 * It looks ugly, but this is the cleanest way to achieve cross platform results.
 * Cleaned up from:
 *
 * http://nadeausoftware.com/articles/2012/09/c_c_tip_how_get_physical_memory_size_system
 *
 * Note that this function:
 * 1) Was released under the following CC attribution license:
 *    http://creativecommons.org/licenses/by/3.0/deed.en_US.
 * 2) Was originally implemented by David Robert Nadeau.
 * 3) Was modified for Redis by Matt Stancliff.
 * 4) This note exists in order to comply with the original license.
 */
size_t hpmalloc_get_memory_size(void) 
{
#if defined(__unix__) || defined(__unix) || defined(unix)
 #if defined(_SC_PHYS_PAGES) && defined(_SC_PAGESIZE)
     /* FreeBSD, Linux, OpenBSD, and Solaris. -------------------- */
     return (size_t)sysconf(_SC_PHYS_PAGES) * (size_t)sysconf(_SC_PAGESIZE);
 #else
     return 0L;          /* Unknown method to get the data. */
 #endif
#else
    return 0L;          /* Unknown OS. */
#endif
}

