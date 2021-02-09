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
int nanosleep (const struct timespec *requested_time, struct [Function]
timespec *remaining)
If resolution to seconds is not enough the nanosleep function can be used. As the
name suggests the sleep interval can be specifed in nanoseconds. The actual elapsed
time of the sleep interval might be longer since the system rounds the elapsed time
you request up to the next integer multiple of the actual resolution the system can
deliver.
*requested_time is the elapsed time of the interval you want to sleep.
The function returns as *remaining the elapsed time left in the interval for which
you requested to sleep. If the interval completed without getting interrupted by a
signal, this is zero.
struct timespec is described in See Section 21.2 [Elapsed Time], page 540.
If the function returns because the interval is over the return value is zero. If the
function returns −1 the global variable errno is set to the following values:
EINTR The call was interrupted because a signal was delivered to the thread. If
the remaining parameter is not the null pointer the structure pointed to
by remaining is updated to contain the remaining elapsed time.
EINVAL The nanosecond value in the requested time parameter contains an illegal
value. Either the value is negative or greater than or equal to 1000 million.
This function is a cancellation point in multi-threaded programs. This is a problem
if the thread allocates some resources (like memory, fle descriptors, semaphores or
whatever) at the time nanosleep is called. If the thread gets canceled these resources
stay allocated until the program ends. To avoid this calls to nanosleep should be
protected using cancellation handlers.
The nanosleep function is declared in ‘time.h’.
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

int nanosleep (const struct timespec *requested_time, struct timespec *remaining);

int better_sleep (double sleep_time)
{
	struct timespec tv;
	/* Construct the timespec from the number of whole seconds... */
	tv.tv_sec = (time_t) sleep_time;
	/* ... and the remainder in nanoseconds. */
	tv.tv_nsec = (long) ((sleep_time - tv.tv_sec) * 1e+9);
	while (1)
	{
		/* Sleep for the time specified in tv. If interrupted by a
		 * signal, place the remaining time left to sleep back into tv. */
		int rval = nanosleep (&tv, &tv);
		if (rval == 0)
			/* Completed the entire sleep time; all done. */
			return 0;
		else if (errno == EINTR)
			/* Interrupted by a signal. Try again. */
			continue;
		else
			/* Some other error; bail out. */
			return rval;
	}
	return 0;
}

int main()
{
	better_sleep(1.001);   
    
    
    return 0;
}
