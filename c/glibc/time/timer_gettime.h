/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */


#include <time.h>

//int timer_settime(timer_t timerid, int flags,
//                  const struct itimerspec *new_value,
//                  struct itimerspec * old_value);
int timer_gettime(timer_t timerid, struct itimerspec *curr_value);

//Link with -lrt.

/* Get the time remaining on a POSIX.1b interval timer. */
SYSCALL_DEFINE2(timer_gettime, timer_t, timer_id,
		struct __kernel_itimerspec __user *, setting)
{
	struct itimerspec64 cur_setting;

	int ret = do_timer_gettime(timer_id, &cur_setting);
	if (!ret) {
		if (put_itimerspec64(&cur_setting, setting))
			ret = -EFAULT;
	}
	return ret;
}

#ifdef CONFIG_COMPAT_32BIT_TIME

SYSCALL_DEFINE2(timer_gettime32, timer_t, timer_id,
		struct old_itimerspec32 __user *, setting)
{
	struct itimerspec64 cur_setting;

	int ret = do_timer_gettime(timer_id, &cur_setting);
	if (!ret) {
		if (put_old_itimerspec32(&cur_setting, setting))
			ret = -EFAULT;
	}
	return ret;
}

#endif

