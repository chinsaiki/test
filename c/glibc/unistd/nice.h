//The kernel uses a system call nice(int increment) to change current process’s priority
//
#include <unistd.h>

//nice
//功能说明：设置优先权。
//语 法：nice [-n <优先等级>][--help][--version][执行指令]
//补充说明：nice 指令可以改变程序执行的优先权等级。
//参 数：-n<优先等级>或-<优先等级>或--adjustment=<优先等级> 设置欲执行的指令的优先权等级。等级
//的范围从-20－19，其中-20 最高，19 最低，只有系统管理者可以设置负数的等级。
int nice(int incr);



#define MAX_NICE	19
#define MIN_NICE	-20
#define NICE_WIDTH	(MAX_NICE - MIN_NICE + 1)

#ifdef __ARCH_WANT_SYS_NICE

/*
 * sys_nice - change the priority of the current process.
 * @increment: priority increment
 *
 * sys_setpriority is a more generic, but much slower function that
 * does similar things.
 */
SYSCALL_DEFINE1(nice, int, increment)
{
	long nice, retval;

	/*
	 * Setpriority might change our priority at the same moment.
	 * We don't have to worry. Conceptually one call occurs first
	 * and we have a single winner.
	 */
	increment = clamp(increment, -NICE_WIDTH, NICE_WIDTH);
	nice = task_nice(current) + increment;

	nice = clamp_val(nice, MIN_NICE, MAX_NICE);
	if (increment < 0 && !can_nice(current, nice))
		return -EPERM;

	retval = security_task_setnice(current, nice);
	if (retval)
		return retval;

	set_user_nice(current, nice);
	return 0;
}

#endif

