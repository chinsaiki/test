/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

#include <time.h>

//clock_nanosleep（）用于要求进程休眠一定时间。
//nanosleep（）也可以用于睡眠。
//但是，nanosleep（）使用的 CLOCK_REALTIME 可以由其他进程更改，因此可以是不连续的，也可以在时间上跳回。
//在clock_nanosleep中，显式指定了 CLOCK_MONOTONIC。
//这是一个不变的时钟，启动后不会改变。通过使用绝对时间指定每个周期的结束来实现周期性。
//有关clock_nanosleep的更多信息，请参见http://man7.org/linux/man-pages/man2/clock_nanosleep.2.html

//nanosleep - high-resolution sleep
int nanosleep(const struct timespec *req, struct timespec *rem);



#ifdef CONFIG_64BIT

SYSCALL_DEFINE2(nanosleep, struct __kernel_timespec __user *, rqtp,
		struct __kernel_timespec __user *, rmtp)
{
	struct timespec64 tu;

	if (get_timespec64(&tu, rqtp))
		return -EFAULT;

	if (!timespec64_valid(&tu))
		return -EINVAL;

	current->restart_block.nanosleep.type = rmtp ? TT_NATIVE : TT_NONE;
	current->restart_block.nanosleep.rmtp = rmtp;
	return hrtimer_nanosleep(timespec64_to_ktime(tu), HRTIMER_MODE_REL,
				 CLOCK_MONOTONIC);
}

#endif


#ifdef CONFIG_COMPAT_32BIT_TIME

SYSCALL_DEFINE2(nanosleep_time32, struct old_timespec32 __user *, rqtp,
		       struct old_timespec32 __user *, rmtp)
{
	struct timespec64 tu;

	if (get_old_timespec32(&tu, rqtp))
		return -EFAULT;

	if (!timespec64_valid(&tu))
		return -EINVAL;

	current->restart_block.nanosleep.type = rmtp ? TT_COMPAT : TT_NONE;
	current->restart_block.nanosleep.compat_rmtp = rmtp;
	return hrtimer_nanosleep(timespec64_to_ktime(tu), HRTIMER_MODE_REL,
				 CLOCK_MONOTONIC);
}
#endif

