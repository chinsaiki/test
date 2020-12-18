#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/debugreg.h>
#include <sys/wait.h>
#include <dirent.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>
#include <errno.h>

#define TCB_IGNORE_ONE_STOP	0x01

struct tcb {
	pid_t pid;
	int flags;
};

struct tcb tcbs[32];

long watch_address;

unsigned nproc;

static struct tcb *alloc_tcb(void)
{
	int i;
	struct tcb *this;

	assert(nproc < 32);
	
	for (i = 0; i < 32; i++) {
		this = &tcbs[i];
		if (this->pid == 0)
			break;
	}

	if (i == 32)
		return NULL;

	nproc++;
	return this;
}

static void drop_tcb(struct tcb *tcb)
{
	memset(tcb, 0, sizeof(struct tcb));
	nproc--;
}

static struct tcb *pid2tcb(pid_t pid)
{
	int i;
	struct tcb *tcb;

	for (i = 0; i < 32; i++) {
		tcb = &tcbs[i];
		if (pid == tcb->pid)
			return tcb;
	}
	return NULL;
}

static void set_hwbp(pid_t pid, long value)
{
	long addr, data;
	int rc;

	printf("set hwbp for %d\n", pid);

	/* 设置一个断点, dr0,断点默认是disabled */
	addr = offsetof(struct user, u_debugreg);
	data = value;
	rc = ptrace(PTRACE_POKEUSER, pid, (void*)addr, (void*)data);
	if (rc) {
		perror("set hwbp");
		exit(-1);
	}

	/* 激活4字节写断点, dr7 */
	addr = offsetof(struct user, u_debugreg) + 7 * sizeof(long);
	data = (0x1 /* enable dr0 */) | ((DR_RW_WRITE | DR_LEN_4) << DR_CONTROL_SHIFT);
	rc = ptrace(PTRACE_POKEUSER, pid, (void*)addr, (void*)data);
	if (rc) {
		perror("enable hwbp");
		exit(-1);
	}
}

static void attach(pid_t pid)
{
	char procdir[32];
	DIR *dir;
	struct dirent *de;
	struct tcb *tcb;
	pid_t tid;
	int rc;

	snprintf(procdir, 32, "/proc/%d/task", pid);
	dir = opendir(procdir);
	assert(dir);
	while (de = readdir(dir)) {
		/* ignore . && .. */
		if (de->d_name[0] == '.')
			continue;
		tid = atoi(de->d_name);
		tcb = alloc_tcb();
		tcb->pid = tid;
		tcb->flags |= TCB_IGNORE_ONE_STOP;
		rc = ptrace(PTRACE_ATTACH, tid, NULL, NULL);
		if (rc) {
			perror("attach\n");
			exit(-1);
		}
	}
	closedir(dir);
}

static int trace(void)
{
	pid_t pid;
	int rc;
	int status;
	unsigned int sig;
	unsigned int event;
	struct tcb *this;
	struct user_regs_struct regs;

	if (nproc == 0)
		return 0;

	pid = waitpid(-1, &status, __WALL);
	if (pid < 0) {
		if (errno == EINTR)
			return 1;
		perror("waitpid");
		return 0;
	}
	this = pid2tcb(pid);
	assert(this);

	event = (unsigned int)status >> 16;
	(void)event;

	if (WIFSIGNALED(status)) {
		printf("%d: signaled\n", pid);
		drop_tcb(this);
		return 1;
	}
	if (WIFEXITED(status)) {
		printf("%d: exited\n", pid);
		drop_tcb(this);
		return 1;
	}
	if (!WIFSTOPPED(status)) {
		printf("%d: strange\n", pid);
		drop_tcb(this);
		return 1;
	}

	sig = WSTOPSIG(status);
	if ((sig == SIGSTOP) && (this->flags & TCB_IGNORE_ONE_STOP)) {
		this->flags &= ~TCB_IGNORE_ONE_STOP;
		printf("%d: stopped by sigstop\n", pid);
		set_hwbp(pid, watch_address);
		goto restart_tracee;
	}

	printf("%d: stopped by signal %d\n", pid, sig);
	rc = ptrace(PTRACE_GETREGS, pid, NULL, &regs);
	if (rc) {
		perror("getregs");
		goto restart_tracee;
	}
	printf("%d: rip 0x%llx\n", pid, regs.rip);

restart_tracee:
	rc = ptrace(PTRACE_CONT, pid, NULL, NULL);
	if (rc) {
		perror("cont");
		return 0;
	}
	return 1;
}

static void detach(void)
{
}

int main(int argc, char **argv)
{
	pid_t pid;
	int rc;
	struct user_regs_struct regs;

	/* $0 pid address */
	assert(argc == 3);

	pid = atoi(argv[1]);
	assert(pid > 1);
	watch_address = strtoul(argv[2], NULL, 16);

	attach(pid);

	while (trace())
		;

	detach();

	return 0;
}


