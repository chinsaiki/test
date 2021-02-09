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
char * ctime (const time t *time) [Function]
The ctime function is similar to asctime, except that you specify the calendar time
argument as a time_t simple time value rather than in broken-down local time format.
It is equivalent to
asctime (localtime (time))
ctime sets the variable tzname, because localtime does so. See Section 21.4.8 [Func
tions and Variables for Time Zones], page 567.


char * ctime_r (const time t *time, char *buffer)

 */

#include <time.h>
#include <stdio.h>
#include <malloc.h>


int main()
{
    time_t t = 12;
    
    
	char *str1 = ctime(&t);

	printf("%s\n", str1);

    char *str2;
    
    str2 = ctime_r(&t, str2);

	printf("%s\n", str2);
    
	return 0;
}

