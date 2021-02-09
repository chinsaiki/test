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
/**
struct tm [Data Type]
This is the data type used to represent a broken-down time. The structure contains
at least the following members, which can appear in any order.
int tm_sec
This is the number of full seconds since the top of the minute (normally
in the range 0 through 59, but the actual upper limit is 60, to allow for
leap seconds if leap second support is available).
int tm_min
This is the number of full minutes since the top of the hour (in the range
0 through 59).
int tm_hour
This is the number of full hours past midnight (in the range 0 through
23).
int tm_mday
This is the ordinal day of the month (in the range 1 through 31). Watch
out for this one! As the only ordinal number in the structure, it is incon
sistent with the rest of the structure.
int tm_mon
This is the number of full calendar months since the beginning of the year
(in the range 0 through 11). Watch out for this one! People usually use
ordinal numbers for month-of-year (where January = 1).
int tm_year
This is the number of full calendar years since 1900.
int tm_wday
This is the number of full days since Sunday (in the range 0 through 6).
int tm_yday
This is the number of full days since the beginning of the year (in the
range 0 through 365).
int tm_isdst
This is a ﬂag that indicates whether Daylight Saving Time is (or was, or
will be) in eﬀect at the time described. The value is positive if Daylight
Saving Time is in eﬀect, zero if it is not, and negative if the information
is not available.
long int tm_gmtoff
This feld describes the time zone that was used to compute this broken
down time value, including any adjustment for daylight saving; it is the
number of seconds that you must add to UTC to get local time. You can
also think of this as the number of seconds east of UTC. For example,
for U.S. Eastern Standard Time, the value is -5*60*60. The tm_gmtoff
feld is derived from BSD and is a GNU library extension; it is not visible
in a strict ISO C environment.
const char *tm_zone
This feld is the name for the time zone that was used to compute this
broken-down time value. Like tm_gmtoff, this feld is a BSD and GNU
extension, and is not visible in a strict ISO C environment.
*/

#include <time.h>


struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
    long int tm_gmtoff;
    const char *tm_zone; 
};