#include <sched.h>

/* Glibc封装的clone。Prototype for the glibc wrapper function */
int clone(int (*fn)(void *), void *child_stack,
         int flags, void *arg, ...
         /* pid_t *ptid, struct user_desc *tls, pid_t *ctid */ );

/* 原始的系统调用 Prototype for the raw system call */
long clone(unsigned long flags, void *child_stack,
         void *ptid, void *ctid,
         struct pt_regs *regs);

