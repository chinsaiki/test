/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 06月 14日 星期五 09:05:28 CST. */
/* 
 Copyright (C) Rong Tao @Beijing

 Permission is granted to copy, distribute and/or modify this document
 under the terms of the GNU Free Documentation License, Version 1.3
 or any later version published by the Free Software Foundation;
 with no Invariant Sections, no Front-Cover Texts, and no Back-Cover
 Texts. A copy of the license is included in the section entitled ‘‘GNU
 Free Documentation License’’.
   2019年 03月 14日 星期四 19:24:32 CST. 
*/
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 13日 星期三 08:54:03 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 08日 星期五 08:10:16 CST. */
/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 07日 星期四 20:27:53 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 07日 星期四 20:26:59 CST. */
/*
The function getrusage and the data type struct rusage are used to examine the resource
usage of a process. They are declared in ‘sys/resource.h’.
int getrusage (int processes, struct rusage *rusage) [Function]
This function reports resource usage totals for processes specifed by processes, storing
the information in *rusage.
In most systems, processes has only two valid values:
RUSAGE_SELF
Just the current process.
RUSAGE_CHILDREN
All child processes (direct and indirect) that have already terminated.
In the GNU system, you can also inquire about a particular child process by specifying
its process ID.
The return value of getrusage is zero for success, and -1 for failure.
EINVAL The argument processes is not valid.
One way of getting resource usage for a particular child process is with the function
wait4, which returns totals for a child when it terminates. See Section 26.8 [BSD Process
Wait Functions], page 696.

*/

//int getrusage (int processes, struct rusage *rusage)

#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>

void print_cpu_time()
{
	struct rusage usage;
	getrusage (RUSAGE_SELF, &usage);
	printf ("CPU time: %ld.%06ld sec user, %ld.%06ld sec system\n",
			usage.ru_utime.tv_sec, usage.ru_utime.tv_usec,
			usage.ru_stime.tv_sec, usage.ru_stime.tv_usec);
}

int show(struct rusage rusage)
{
    printf("ru_utime.tv_sec: %d\n", rusage.ru_utime.tv_sec);
    printf("ru_utime.tv_usec: %d\n", rusage.ru_utime.tv_usec);
    printf("ru_stime.tv_sec: %d\n", rusage.ru_stime.tv_sec);
    printf("ru_stime.tv_usec: %d\n", rusage.ru_stime.tv_usec);
    printf("ru_maxrss: %d\n", rusage.ru_maxrss);
    printf("ru_ixrss: %d\n", rusage.ru_ixrss);
    printf("ru_idrss: %d\n", rusage.ru_idrss);
    printf("ru_isrss: %d\n", rusage.ru_isrss);
    printf("ru_minflt: %d\n", rusage.ru_minflt);
    printf("ru_majflt: %d\n", rusage.ru_majflt);
    printf("ru_nswap: %d\n", rusage.ru_nswap);
    printf("ru_inblock: %d\n", rusage.ru_inblock);
    printf("ru_oublock: %d\n", rusage.ru_oublock);
    printf("ru_msgsnd: %d\n", rusage.ru_msgsnd);
    printf("ru_msgrcv: %d\n", rusage.ru_msgrcv);
    printf("ru_nsignals: %d\n", rusage.ru_nsignals);
    printf("ru_nvcsw: %d\n", rusage.ru_nvcsw);
    printf("ru_nivcsw: %d\n", rusage.ru_nivcsw);
}

int main()
{
	print_cpu_time();

    struct rusage rsue, rsue2;
    
    getrusage(getpid(), &rsue);
    getrusage(getpid(), &rsue2);
    show(rsue);
    
	float user, sys;
	user = (float)rsue.ru_utime.tv_sec + rsue.ru_utime.tv_usec / 1000000.0;
	user+= (float)rsue2.ru_utime.tv_sec + rsue2.ru_utime.tv_usec / 1000000.0;

	sys = (float)rsue.ru_stime.tv_sec + rsue.ru_stime.tv_usec / 1000000.0;
	sys+= (float)rsue2.ru_stime.tv_sec + rsue2.ru_stime.tv_usec / 1000000.0;

	printf("user: %g, sys time: %g\n", user, sys);

	/**
	 *	The getrusage function is called twice to return the resource utilization of both the calling process (RUSAGE_SELF) and all the terminated children of the calling process (RUSAGE_CHILDREN). The values printed are the total user time (CPU time spent in the user process) and total system time (CPU time spent within the kernel, executing on behalf of the calling process).
	 */
    
    return 0;
}
