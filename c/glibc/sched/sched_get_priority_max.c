/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 06月 14日 星期五 09:05:21 CST. */
/* 
 Copyright (C) Rong Tao @Beijing

 Permission is granted to copy, distribute and/or modify this document
 under the terms of the GNU Free Documentation License, Version 1.3
 or any later version published by the Free Software Foundation;
 with no Invariant Sections, no Front-Cover Texts, and no Back-Cover
 Texts. A copy of the license is included in the section entitled ‘‘GNU
 Free Documentation License’’.
   2019年 03月 14日 星期四 19:24:29 CST. 
*/
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 13日 星期三 08:54:00 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 08日 星期五 08:10:13 CST. */
/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 07日 星期四 20:27:51 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 07日 星期四 20:26:57 CST. */
/*
 *	int sched_get_priority_max (int *policy); [Function]
 *	This function returns the highest absolute priority value that is allowable for a process
 *	that with scheduling policy policy.
 *	On Linux, it is 0 for SCHED OTHER and 99 for everything else.
 *	On success, the return value is 0. Otherwise, it is -1 and ERRNO is set accordingly.
 *	The errno values specifc to this function are:
 *	EINVAL policy does not identify an existing scheduling policy.
 */
#include <sched.h>
#include <stdio.h>
#include <unistd.h>
#include <sched.h>

int main()
{
  printf("Valid priority range for SCHED_OTHER: %d - %d\n",
         sched_get_priority_min(SCHED_OTHER),
         sched_get_priority_max(SCHED_OTHER));

  printf("Valid priority range for SCHED_FIFO: %d - %d\n",
         sched_get_priority_min(SCHED_FIFO),
         sched_get_priority_max(SCHED_FIFO));

  printf("Valid priority range for SCHED_RR: %d - %d\n",
         sched_get_priority_min(SCHED_RR),
         sched_get_priority_max(SCHED_RR));
}

