/**
 *  cas-queue.h
 *  
 *  提供简单低时延的 基于 CAS 操作的队列
 *  
 *  荣涛  2021年1月13日
 *  
 */
#ifndef __CAS_QUEUE_H
#define __CAS_QUEUE_H 1

#ifndef __x86_64__
# error "Not support Your Arch, just support x86-64"
#endif

#define RDTSC() ({\
    register uint32_t a,d; \
    __asm__ __volatile__( "rdtsc" : "=a"(a), "=d"(d)); \
    (((uint64_t)a)+(((uint64_t)d)<<32)); \
    })

#define CAS(ptr, val_old, val_new) ({ \
    char ret; \
    __asm__ __volatile__("lock; "\
        "cmpxchgl %2,%0; setz %1"\
        : "+m"(*ptr), "=q"(ret)\
        : "r"(val_new),"a"(val_old)\
        : "memory"); \
    ret;})


typedef enum  {
    READY_TO_ENQUEUE = 1,
    READY_TO_DEQUEUE = 2,
}QUEUE_OPS;


#define PAGE_SIZE 4096
#define CACHE_LINE_SIZE 64
#define CACHE_ALIGNED __attribute__((aligned(CACHE_LINE_SIZE)))
#define DOUBLE_CACHE_ALIGNED __attribute__((aligned(2 * CACHE_LINE_SIZE)))




#endif /*<__CAS_QUEUE_H>*/
