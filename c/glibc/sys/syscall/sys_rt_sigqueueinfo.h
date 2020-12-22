#include <sys/syscall.h>

//sys_rt_sigqueueinfo - send signal information to a signal
long sys_rt_sigqueueinfo(pid_t pid, int sig, siginfo_t __user * uinfo);

//可以向下面这样使用
static int sys_rt_sigqueueinfo(pid_t tgid, int sig, siginfo_t *uinfo)
{
	return syscall(SYS_rt_sigqueueinfo, tgid, sig, uinfo);
}
//see also rt_sigqueueinfo()

