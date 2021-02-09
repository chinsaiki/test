/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */


#include <time.h>

int timer_settime(timer_t timerid, int flags,
                  const struct itimerspec *new_value,
                  struct itimerspec * old_value);
//int timer_gettime(timer_t timerid, struct itimerspec *curr_value);

//Link with -lrt.


/* Set a POSIX.1b interval timer */
SYSCALL_DEFINE4(timer_settime, timer_t, timer_id, int, flags,
		const struct __kernel_itimerspec __user *, new_setting,
		struct __kernel_itimerspec __user *, old_setting)
{
	struct itimerspec64 new_spec, old_spec;
	struct itimerspec64 *rtn = old_setting ? &old_spec : NULL;
	int error = 0;

	if (!new_setting)
		return -EINVAL;

	if (get_itimerspec64(&new_spec, new_setting))
		return -EFAULT;

	error = do_timer_settime(timer_id, flags, &new_spec, rtn);
	if (!error && old_setting) {
		if (put_itimerspec64(&old_spec, old_setting))
			error = -EFAULT;
	}
	return error;
}

