#ifndef __x86_64__
# error "Not support Your Arch, just support x86-64"
#endif

#include <errno.h>
#include <assert.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <syscall.h>
#include <sys/types.h>

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


#define gettid() syscall(__NR_gettid)

#define log_enqueue(fmt...)  do{printf("\033[33m[%d]", gettid());printf(fmt);printf("\033[m");}while(0)
#define log_dequeue(fmt...)  do{printf("\033[32m[%d]", gettid());printf(fmt);printf("\033[m");}while(0)

