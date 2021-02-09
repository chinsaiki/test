/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

#include <time.h>

//获取从1970.01.01 00:00:00:00开始的秒数
time_t time(time_t *t);


/*
 * sys_time() can be implemented in user-level using
 * sys_gettimeofday().  Is this for backwards compatibility?  If so,
 * why not move it into the appropriate arch directory (for those
 * architectures that need it).
 */
SYSCALL_DEFINE1(time, __kernel_old_time_t __user *, tloc)
{
	__kernel_old_time_t i = (__kernel_old_time_t)ktime_get_real_seconds();

	if (tloc) {
		if (put_user(i,tloc))
			return -EFAULT;
	}
	force_successful_syscall_return();
	return i;
}

