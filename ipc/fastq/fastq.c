/**********************************************************************************************************************\
*  文件： fastq.c
*  介绍： 低时延队列
*  作者： 荣涛
*  日期：
*       2021年1月25日    创建与开发轮询功能
*       2021年1月27日 添加 通知+轮询 功能接口，消除零消息时的 CPU100% 问题
*       2021年1月28日 调整代码格式，添加必要的注释
*       2021年2月1日 添加多入单出队列功能 ： 采用 epoll 实现
*       2021年2月2日 添加统计功能接口，尽可能减少代码量
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

#include "fastq.h"
#include "atomic.h"



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

/**
 *  原始接口
 */
#define _FQ_NAME(name)   name

#include "fastq_compat.c"

/**
 *  支持统计功能的接口
 */
#undef _FQ_NAME
#define _FQ_NAME(name)   name##Stats
#define FASTQ_STATISTICS //统计功能

#include "fastq_compat.c"


