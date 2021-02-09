/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

#include <signal.h>
#include <time.h>
#include <stdio.h>

/* 回调函数 */
void sigev_notify_fn(union sigval sv){
    timer_t *ptimerid = (timer_t *)sv.sival_ptr;
    int cnt = timer_getoverrun(*ptimerid);
    printf("CB: %ld, %d\n", *ptimerid, cnt);
}


int main()
{
    struct itimerspec it;
    timer_t timerid;
    struct sigevent sev;
    int ret;

    /* 填充sigevent结构 */
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_value.sival_ptr = &timerid;

    sev.sigev_notify_function = sigev_notify_fn;
    sev.sigev_notify_attributes = NULL;

    /* 创建定时器 */
    timer_create(CLOCK_REALTIME, &sev, &timerid);

    printf("timer_create: timerid  = %ld\n", timerid);

    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    
    struct itimerspec newit;
    
    newit.it_value.tv_sec = tp.tv_sec+2;/* 这里如果不是这个数值，将马上超时回调 */
    newit.it_value.tv_nsec = tp.tv_nsec;
    newit.it_interval.tv_sec = 2;
    newit.it_interval.tv_nsec = 0;

    /* 设置定时器时间 */
    ret = timer_settime(timerid, TIMER_ABSTIME, &newit, NULL);
    if(ret<0){
        perror("timer_settime:");
    }
    printf("timer_settime: val(%ld,%ld), inte(%ld,%ld)\n", 
            newit.it_value.tv_sec, newit.it_value.tv_nsec,
            newit.it_interval.tv_sec, newit.it_interval.tv_nsec);

    /* 放置主循环退出 */
    while(1)sleep(1);

    /* 删除定时器 */
    timer_delete(timerid);
}



