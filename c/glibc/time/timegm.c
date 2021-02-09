/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 06月 14日 星期五 09:05:33 CST. */
/* 
 Copyright (C) Rong Tao @Beijing

 Permission is granted to copy, distribute and/or modify this document
 under the terms of the GNU Free Documentation License, Version 1.3
 or any later version published by the Free Software Foundation;
 with no Invariant Sections, no Front-Cover Texts, and no Back-Cover
 Texts. A copy of the license is included in the section entitled ‘‘GNU
 Free Documentation License’’.
   2019年 03月 14日 星期四 19:24:34 CST. 
*/
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 13日 星期三 08:54:06 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 08日 星期五 08:10:18 CST. */
/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 07日 星期四 20:27:55 CST. */
/*
time_t timegm (struct tm *brokentime) [Function]
timegm is functionally identical to mktime except it always takes the input values to
be Coordinated Universal Time (UTC) regardless of any local time zone setting.
Note that timegm is the inverse of gmtime.
Portability note: mktime is essentially universally available. timegm is rather rare.
For the most portable conversion from a UTC broken-down time to a simple time,
set the TZ environment variable to UTC, call mktime, then set TZ back.
*/
#include <stdio.h>
#include <time.h>


int main()
{
    time_t time = 19;
    struct tm *t = localtime(&time);
    
    struct tm *t2;
    t2 = gmtime_r(&time, t2);
    
    printf("tm_sec = %d:%d\n", t->tm_sec, t2->tm_sec);
    printf("tm_min = %d:%d\n", t->tm_min, t2->tm_min);
    printf("tm_hour = %d:%d\n", t->tm_hour, t2->tm_hour);
    printf("tm_mday = %d:%d\n", t->tm_mday, t2->tm_mday);
    printf("tm_mon = %d:%d\n", t->tm_mon, t2->tm_mon);
    printf("tm_year = %d:%d\n", t->tm_year, t2->tm_year);
    printf("tm_wday = %d:%d\n", t->tm_wday, t2->tm_wday);
    printf("tm_yday = %d:%d\n", t->tm_yday, t2->tm_yday);
    printf("tm_isdst = %d:%d\n", t->tm_isdst, t2->tm_isdst);
    printf("tm_gmtoff = %d:%d\n", t->tm_gmtoff, t2->tm_gmtoff);
    printf("tm_zone = %s:%s\n", t->tm_zone, t2->tm_zone);
    
    
    struct tm t3;
    time_t time2 = timegm(t2);
    
    printf("---\n");
    printf("time2: %d\n", time2);
    
    return 0;
}