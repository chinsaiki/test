#include <stdio.h>
#include <sched.h>

//该SCHED_RR政策与政策略有不同SCHED_FIFO。SCHED_RR在循环循环中分配具有相同优先级的并发进程。
//    这样，为每个进程分配了时间片。的sched_rr_get_interval()函数将报告已分配给每个进程的时间片。
//即使POSIX要求此功能只能与SCHED_RR进程一起使用，该sched_rr_get_interval()功能仍可以检索Linux上任何进程的时间片长度。
//返回的时间片信息为timespec，即自1970年1月1日格林尼治标准时间00:00:00以来的秒数和纳秒数：


//以下命令sched_03以不同的策略和优先级运行测试程序。
//具有SCHED_FIFO策略的进程将返回0秒和0纳秒的时间片，表明它是无限的：

//$ chrt -o 0 ./sched_03
//Timeslice: 0.38994072

//$ chrt -r 10 ./sched_03
//Timeslice: 0.99984800

//$ chrt -f 10 ./sched_03
//Timeslice: 0.0

//-f, --fifo  Set scheduling policy to SCHED_FIFO.
//-r, --rr    Set scheduling policy to SCHED_RR.  When policy is not defined the SCHED_RR is used as default.
//-o, --other Set policy scheduling policy to SCHED_OTHER.

int main()
{
   struct timespec ts;
   int ret;

   /* real apps must check return values */
   ret = sched_rr_get_interval(getpid(), &ts);

   printf("Timeslice: %lu.%lu\n", ts.tv_sec, ts.tv_nsec);
}

