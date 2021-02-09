/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

#include <time.h>

//clock_nanosleep（）用于要求进程休眠一定时间。
//nanosleep（）也可以用于睡眠。
//但是，nanosleep（）使用的 CLOCK_REALTIME 可以由其他进程更改，因此可以是不连续的，也可以在时间上跳回。
//在clock_nanosleep中，显式指定了 CLOCK_MONOTONIC。
//这是一个不变的时钟，启动后不会改变。通过使用绝对时间指定每个周期的结束来实现周期性。
//有关clock_nanosleep的更多信息，请参见http://man7.org/linux/man-pages/man2/clock_nanosleep.2.html


//clock_nanosleep - high-resolution sleep with specifiable clock
int clock_nanosleep(clockid_t clock_id, int flags,
                       const struct timespec *request,
                       struct timespec *remain);

//Link with -lrt (only for glibc versions before 2.17).
//flags
//CLOCK_REALTIME   A settable system-wide real-time clock.
//
//CLOCK_MONOTONIC  A nonsettable, monotonically increasing clock that measures time since some  unspecified
//               point in the past that does not change after system startup.
//
//CLOCK_PROCESS_CPUTIME_ID
//               A  settable  per-process  clock  that  measures  CPU time consumed by all threads in the
//               process.



SYSCALL_DEFINE4(clock_nanosleep, const clockid_t, which_clock, int, flags,
		const struct __kernel_timespec __user *, rqtp,
		struct __kernel_timespec __user *, rmtp)
{
	const struct k_clock *kc = clockid_to_kclock(which_clock);
	struct timespec64 t;

	if (!kc)
		return -EINVAL;
	if (!kc->nsleep)
		return -EOPNOTSUPP;

	if (get_timespec64(&t, rqtp))
		return -EFAULT;

	if (!timespec64_valid(&t))
		return -EINVAL;
	if (flags & TIMER_ABSTIME)
		rmtp = NULL;
	current->restart_block.nanosleep.type = rmtp ? TT_NATIVE : TT_NONE;
	current->restart_block.nanosleep.rmtp = rmtp;

	return kc->nsleep(which_clock, flags, &t);
}

#ifdef CONFIG_COMPAT_32BIT_TIME

SYSCALL_DEFINE4(clock_nanosleep_time32, clockid_t, which_clock, int, flags,
		struct old_timespec32 __user *, rqtp,
		struct old_timespec32 __user *, rmtp)
{
	const struct k_clock *kc = clockid_to_kclock(which_clock);
	struct timespec64 t;

	if (!kc)
		return -EINVAL;
	if (!kc->nsleep)
		return -EOPNOTSUPP;

	if (get_old_timespec32(&t, rqtp))
		return -EFAULT;

	if (!timespec64_valid(&t))
		return -EINVAL;
	if (flags & TIMER_ABSTIME)
		rmtp = NULL;
	current->restart_block.nanosleep.type = rmtp ? TT_COMPAT : TT_NONE;
	current->restart_block.nanosleep.compat_rmtp = rmtp;

	return kc->nsleep(which_clock, flags, &t);
}

#endif

