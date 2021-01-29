
#include <string.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <signal.h>
#include <stdio.h>
#include "strace.h"
#include "syscalls.h"

static int	get_stopsig(int pid, char **strtab)
{
  siginfo_t	sig;

  (void)strtab;
  sig.si_signo = 0;
  if (-1 == ptrace(PTRACE_GETSIGINFO, pid, NULL, &sig))
    return fprintf(stderr, "getsiginfo fail\n");
  switch (sig.si_signo) {
  case 0:
  case 5:
  case 17:
  case 18:
  case 19:
  case 20:
  case 25:
  case 28:
    return 0;
  }
  fprintf(stderr, "Killed by signal %d\n",
	  sig.si_signo); /* , strerror(sig.si_errno)); */
  return 1;
}

static struct syscalls	*get_call_infos(const char *name)
{
  size_t	i = 0;

  while (g_syscalls[i].name && strcmp(g_syscalls[i].name, name))
    ++i;
  if (g_syscalls[i].name)
    return &g_syscalls[i];
  return NULL;
}

static int		get_syscall(int pid, char **strtab)
{
  static char		first_call = 1;
  struct user		infos;
  struct syscalls	*call;
  long			word;
  int			status;

  /* Return call informations */
  if (ptrace(PTRACE_GETREGS, pid, NULL, &infos) == -1)
    {      fprintf(stderr, "getregs fail\n");      return 1;    }
  word = ptrace(PTRACE_PEEKTEXT, pid, infos.regs.rip, NULL);
  if ((word & 0xFFFF) != 0x050F)
    return 0;
  if (!(call = get_call_infos(strtab[(int)infos.regs.rax])))
    {
      fprintf(stderr, "Unimplemented syscall : %s\n",
	      strtab[(int)infos.regs.rax]);
      return 0;
    }
  if (first_call && !(first_call = 0))
    return 0; // first call is our strace() call
  fprintf(stderr, "%s(", call->name);
  print_args(call->name, call->p, infos, pid);
  fprintf(stderr, ")");
  fflush(stderr);
  /* Go to return value */
  ptrace(PTRACE_SINGLESTEP, pid, NULL, 0);
  wait4(pid, &status, WUNTRACED, NULL);
  if (ptrace(PTRACE_GETREGS, pid, NULL, &infos) == -1)
    {      fprintf(stderr, "\t= ?\n");      return 1;    }
  if (strchr(call->rtype, '*')) {
    fprintf(stderr, "\t= %#lx", infos.regs.rax);
  } else {
    if ((int)infos.regs.rax < 0)
      fprintf(stderr, "\t= %d (%s)", -1, strerror(-infos.regs.rax));
    else
      fprintf(stderr, "\t= %ld", infos.regs.rax);
  }
  fprintf(stderr, "\n");
  return 0;
}

static void	trace_process(int pid, char **strtab)
{
  int		status;

  while (1)
    {
      wait4(pid, &status, WUNTRACED, NULL);
      if (get_stopsig(pid, strtab))
      	break;
      if (status == 0)
	break;
      if (get_syscall(pid, strtab))
	break ;
      ptrace(PTRACE_SINGLESTEP, pid, NULL, 0);
    }
}

void		exec_parent(int pid, char **strtab, char flag)
{
  if (ptrace(PTRACE_ATTACH, pid, NULL, 0) == -1)
    {
      if (flag)
	kill(pid, SIGKILL);
      exit_error("Cannot attach parent process");
    }
  trace_process(pid, strtab);
}
