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
struct tm * localtime (const time t *time) [Function]
The localtime function converts the simple time pointed to by time to broken-down
time representation, expressed relative to the user’s specifed time zone.
The return value is a pointer to a static broken-down time structure, which might
be overwritten by subsequent calls to ctime, gmtime, or localtime. (But no other
library function overwrites the contents of this object.)
The return value is the null pointer if time cannot be represented as a broken-down
time; typically this is because the year cannot ft into an int.
Calling localtime has one other eﬀect: it sets the variable tzname with information
about the current time zone. See Section 21.4.8 [Functions and Variables for Time
Zones], page 567.
Using the localtime function is a big problem in multi-threaded programs. The result
is returned in a static buﬀer and this is used in all threads. POSIX.1c introduced a variant
of this function.


struct tm * localtime_r (const time t *time, struct tm *resultp)

*/
#include <stdio.h>
#include <time.h>


int main()
{
    time_t time = 1;
    struct tm *t = localtime(&time);
    
    struct tm *t2;
    t2 = localtime_r(&time, t2);
    
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
    
    return 0;
}