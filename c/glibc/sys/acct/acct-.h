#include <unistd.h>

#define ACCT_COMM 16

typedef u_int16_t comp_t;

struct acct {
    char ac_flag; /* 记账标记 */
    u_int16_t ac_uid; /* 记账用户 ID */
    u_int16_t ac_gid; /* 记账组 ID */
    u_int16_t ac_tty; /* 控制终端 */
    u_int32_t ac_btime; /* 进程创建时间 (从开机起的秒数) */
    comp_t ac_utime; /* 用户 CPU 时间 */
    comp_t ac_stime; /* 系统 CPU 时间 */
    comp_t ac_etime; /* 流失的时间 */
    comp_t ac_mem; /* 平均内存用量 (kB) */
    comp_t ac_io; /* Characters transferred (未使用) */
    comp_t ac_rw; /* 读写的块 (未使用) */
    comp_t ac_minflt; /* Minor page faults */
    comp_t ac_majflt; /* Major page faults */
    comp_t ac_swaps; /* Number of swaps (unused) */
    u_int32_t ac_exitcode; /* 进程终止状态 (参看 wait(2)) */
    char ac_comm[ACCT_COMM+1];
    /* 命令名 (执行文件名；以0结尾) */
    char ac_pad[X]; /* 填充字节 */
};

enum { /* 在 ac_flag 域可以设置的位 */
    AFORK = 0x01, /* 调用 fork 之后，但还没有调用 exec */
    ASU = 0x02, /* 使用超级用户权限 */
    ACORE = 0x08, /* 核心转储 */
    AXSIG = 0x10 /* 由信号杀死 */
};


////acct系统调用可用于启动/禁止进程信息记录功能。
////switch process accounting on or off
int acct(const char *filename);
//在进程记账开启之后，每当系统内有进程终止内核将写一个记录到记账文件。
//与现有的文件名作为参数调用时，占被打开，每个终止的进程的记录，被追加到文件名作为终止。参数为NULL 引起占用被关闭。



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

