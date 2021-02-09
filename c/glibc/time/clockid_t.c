/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */


#  ifdef __USE_POSIX199309
/* Identifier for system-wide realtime clock.  */
#   define CLOCK_REALTIME		0
/* Monotonic system-wide clock.  */
#   define CLOCK_MONOTONIC		1
//有多种获取当前时间的方法– gettimeofday，time，clock_gettime和其他一些特定于处理器的实现。
//    其中一些，例如gettimeofday，将从系统时钟获取时间。系统时钟可以由其他进程修改。
//    这意味着时钟可以回到过去。可以使用带有CLOCK_MONOTONIC时钟的clock_gettime来避免此问题。
//    CLOCK_MONOTONIC 参数确保我们获得一个不可设置的单调递增时钟，该时钟测量从过去某个未指定点开始的时间，
//    该时间在系统启动后不会更改[1]。确保不浪费大量CPU周期来获取当前时间也很重要。
//    获得当前时间的特定于CPU的实现将在这里有所帮助。


/* High-resolution timer from the CPU.  */
#   define CLOCK_PROCESS_CPUTIME_ID	2
/* Thread-specific CPU-time clock.  */
#   define CLOCK_THREAD_CPUTIME_ID	3
/* Monotonic system-wide clock, not adjusted for frequency scaling.  */
#   define CLOCK_MONOTONIC_RAW		4
/* Identifier for system-wide realtime clock, updated only on ticks.  */
#   define CLOCK_REALTIME_COARSE	5
/* Monotonic system-wide clock, updated only on ticks.  */
#   define CLOCK_MONOTONIC_COARSE	6
/* Monotonic system-wide clock that includes time spent in suspension.  */
#   define CLOCK_BOOTTIME		7
/* Like CLOCK_REALTIME but also wakes suspended system.  */
#   define CLOCK_REALTIME_ALARM		8
/* Like CLOCK_BOOTTIME but also wakes suspended system.  */
#   define CLOCK_BOOTTIME_ALARM		9
/* Like CLOCK_REALTIME but in International Atomic Time.  */
#   define CLOCK_TAI			11

/* Flag to indicate time is absolute.  */
#   define TIMER_ABSTIME		1
#  endif


void print_clockid(clockid_t clockid){
    switch(clockid){
    /* Identifier for system-wide realtime clock.  */
    case CLOCK_REALTIME: printf("Identifier for system-wide realtime clock.\n");break;
    /* Monotonic system-wide clock.  */
    case CLOCK_MONOTONIC: printf("Monotonic system-wide clock.\n");break;
    /* High-resolution timer from the CPU.  */
    case CLOCK_PROCESS_CPUTIME_ID: printf("High-resolution timer from the CPU.\n");break;
    /* Thread-specific CPU-time clock.  */
    case CLOCK_THREAD_CPUTIME_ID: printf("Thread-specific CPU-time clock.\n");break;

    default: printf("Unknown clock.\n");break;
    }
}

