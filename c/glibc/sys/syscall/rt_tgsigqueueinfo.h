#include <sys/syscall.h>

//查询一个信号和数据       queue a signal and data
int rt_sigqueueinfo(pid_t tgid, int sig, siginfo_t *uinfo);

int rt_tgsigqueueinfo(pid_t tgid, pid_t tid, int sig,
                     siginfo_t *uinfo);

//see also sys_rt_sigqueueinfo()
