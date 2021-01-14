#include <unistd.h>

////acct系统调用可用于启动/禁止进程信息记录功能。
////switch process accounting on or off
int acct(const char *filename);



/**
 * sys_acct - enable/disable process accounting
 * @name: file name for accounting records or NULL to shutdown accounting
 *
 * Returns 0 for success or negative errno values for failure.
 *
 * sys_acct() is the only system call needed to implement process
 * accounting. It takes the name of the file where accounting records
 * should be written. If the filename is NULL, accounting will be
 * shutdown.
 */
SYSCALL_DEFINE1(acct, const char __user *, name)
{
	int error = 0;

	if (!capable(CAP_SYS_PACCT))
		return -EPERM;

	if (name) {
		struct filename *tmp = getname(name);

		if (IS_ERR(tmp))
			return PTR_ERR(tmp);
		mutex_lock(&acct_on_mutex);
		error = acct_on(tmp);
		mutex_unlock(&acct_on_mutex);
		putname(tmp);
	} else {
		rcu_read_lock();
		pin_kill(task_active_pid_ns(current)->bacct);
	}

	return error;
}

