#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#define RTE_LOG(MOD, LEVEL, fmt...) printf(fmt)

#define __rte_unused

static struct sigaction sigbus_action_old;

static int sigbus_need_recover;


static void
sigbus_action_recover(void)
{
	if (sigbus_need_recover) {
		sigaction(SIGBUS, &sigbus_action_old, NULL);
		sigbus_need_recover = 0;
	}
}

/*
 * CPU context registers
 */
struct ctx {
	void	*rsp;		/* 0  */
	void	*rbp;		/* 8  */
	void	*rip;		/* 16 */
	void	*rbx;		/* 24 */
	void	*r12;		/* 32 */
	void	*r13;		/* 40 */
	void	*r14;		/* 48 */
	void	*r15;		/* 56 */
};


void
ctx_switch(struct ctx *new_ctx, struct ctx *curr_ctx);

int
rte_bus_sigbus_handler(const void *failure_addr)
{
	printf("failure_addr = %p.\n", failure_addr);
}

static void sigbus_handler(int signum, siginfo_t *info,
				void __rte_unused *ctx )
{
	int ret;

	RTE_LOG(DEBUG, EAL, "Thread catch SIGBUS, fault address:%p\n",
		info->si_addr);

//	rte_spinlock_lock(&failure_handle_lock);
	ret = rte_bus_sigbus_handler(info->si_addr);
//	rte_spinlock_unlock(&failure_handle_lock);
	if (ret == -1) {
		exit(EXIT_FAILURE);
	} else if (ret == 1) {
		if (sigbus_action_old.sa_flags == SA_SIGINFO
		    && sigbus_action_old.sa_sigaction) {
			(*(sigbus_action_old.sa_sigaction))(signum,
							    info, ctx);
		} else if (sigbus_action_old.sa_flags != SA_SIGINFO
			   && sigbus_action_old.sa_handler) {
			(*(sigbus_action_old.sa_handler))(signum);
		} else {
			exit(EXIT_FAILURE);
		}
	}

	RTE_LOG(DEBUG, EAL, "Success to handle SIGBUS for hot-unplug!\n");
}

                
int
dev_sigbus_handler_register(void)
{
    sigset_t mask;
    struct sigaction action;

    errno = 0;

    if (sigbus_need_recover)
        return 0;

    sigemptyset(&mask);
    sigaddset(&mask, SIGBUS);
    action.sa_flags = SA_SIGINFO;
    action.sa_mask = mask;
    action.sa_sigaction = sigbus_handler;
    sigbus_need_recover = !sigaction(SIGBUS, &action, &sigbus_action_old);

    return errno;
}

int main()
{
    dev_sigbus_handler_register();

    while(1)sleep(1);
}

