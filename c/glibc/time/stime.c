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
 *	time_t time (time t *result) [Function]
 *	The time function returns the current calendar time as a value of type time_t. If
 *	the argument result is not a null pointer, the calendar time value is also stored in
 *	*result. If the current calendar time is not available, the value (time_t)(-1) is
 *	returned.
 *
 *	int stime (time t *newtime) [Function]
 *	stime sets the system clock, i.e., it tells the system that the current calendar time is
 *	newtime, where newtime is interpreted as described in the above defnition of time_t.
 *	settimeofday is a newer function which sets the system clock to better than
 *	one second precision. settimeofday is generally a better choice than stime. See
 *	Section 21.4.2 [High-Resolution Calendar], page 545.
 *	Only the superuser can set the system clock.
 *	If the function succeeds, the return value is zero. Otherwise, it is -1 and errno is set
 *	accordingly:
 *	EPERM The process is not superuser.
 *
 */

#include <time.h>
#include <stdio.h>


int main()
{
	time_t t = time(NULL);

	printf("%d\n", t);

	/*need root user to do this stime()*/
	printf("stime ret: %d\n", stime(&t));

	return 0;
}

