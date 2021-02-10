/* System-dependent timing definitions.  Linux version.
   Copyright (C) 1996,1997,1999-2003,2010,2011,2012 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

/*
 * Never include this file directly; use <time.h> instead.
 */

#if defined __need_timeval || defined __USE_GNU
# ifndef _STRUCT_TIMEVAL
#  define _STRUCT_TIMEVAL	1
#  include <bits/types.h>

/* A time value that is accurate to the nearest
   microsecond but also has a range of years.  */
struct timeval
  {
    __time_t tv_sec;		/* Seconds.  */
    __suseconds_t tv_usec;	/* Microseconds.  */
  };
# endif	/* struct timeval */
#endif

#ifndef __need_timeval
# ifndef _BITS_TIME_H
#  define _BITS_TIME_H	1

/* ISO/IEC 9899:1990 7.12.1: <time.h>
   The macro `CLOCKS_PER_SEC' is the number per second of the value
   returned by the `clock' function. */
/* CAE XSH, Issue 4, Version 2: <time.h>
   The value of CLOCKS_PER_SEC is required to be 1 million on all
   XSI-conformant systems. */
#  define CLOCKS_PER_SEC  1000000l

#  if (!defined __STRICT_ANSI__ || defined __USE_POSIX) \
   && !defined __USE_XOPEN2K
/* Even though CLOCKS_PER_SEC has such a strange value CLK_TCK
   presents the real value for clock ticks per second for the system.  */
#   include <bits/types.h>
extern long int __sysconf (int);
#   define CLK_TCK ((__clock_t) __sysconf (2))	/* 2 is _SC_CLK_TCK */
#  endif

#  ifdef __USE_POSIX199309
/* Identifier for system-wide realtime clock.  */
#   define CLOCK_REALTIME		0
//CLOCK_REALTIME：它代表现实世界中的时间，也称为“墙上时间”，表示从墙上的时钟读取的时间。
//    该时钟用于给事件加时间戳，以及与用户进行接口时。可以由具有正确权限的用户修改。
//    但是，应谨慎使用用户修改，因为如果时钟的值在两次读取之间改变，则可能导致错误的数据。

/* Monotonic system-wide clock.  */
#   define CLOCK_MONOTONIC		1
//有多种获取当前时间的方法– gettimeofday，time，clock_gettime和其他一些特定于处理器的实现。
//    其中一些，例如gettimeofday，将从系统时钟获取时间。系统时钟可以由其他进程修改。
//    这意味着时钟可以回到过去。可以使用带有CLOCK_MONOTONIC时钟的clock_gettime来避免此问题。
//    CLOCK_MONOTONIC 参数确保我们获得一个不可设置的单调递增时钟，该时钟测量从过去某个未指定点开始的时间，
//    该时间在系统启动后不会更改[1]。确保不浪费大量CPU周期来获取当前时间也很重要。
//    获得当前时间的特定于CPU的实现将在这里有所帮助。

//CLOCK_MONOTONIC：表示系统启动后单调增加的时间。该时钟无法通过任何进程设置，
//    它是计算事件之间时间差的首选时钟。本节中的以下示例CLOCK_MONOTONIC用作POSIX时钟。


/* High-resolution timer from the CPU.  */
#   define CLOCK_PROCESS_CPUTIME_ID	2
/* Thread-specific CPU-time clock.  */
#   define CLOCK_THREAD_CPUTIME_ID	3
/* Monotonic system-wide clock, not adjusted for frequency scaling.  */
#   define CLOCK_MONOTONIC_RAW		4
/* Identifier for system-wide realtime clock, updated only on ticks.  */
#   define CLOCK_REALTIME_COARSE	5

/* Monotonic system-wide clock, updated only on ticks.  */
//所述_COARSE时钟具有一个一毫秒的精度，因此只的前三个数字tv_nsec的timespec结构的场是显著。
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

#  ifdef __USE_GNU
#   include <bits/timex.h>

__BEGIN_DECLS

/* Tune a POSIX clock.  */
extern int clock_adjtime (__clockid_t __clock_id, struct timex *__utx) __THROW;

__END_DECLS
#  endif /* use GNU */

# endif	/* bits/time.h */
#endif

#undef __need_timeval
