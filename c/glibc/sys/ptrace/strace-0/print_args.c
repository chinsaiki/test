
#include <string.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include "strace.h"

static unsigned long	get_reg(struct user info, int i)
{
  if (i == 0)
    return (info.regs.rdi);
  if (i == 1)
    return (info.regs.rsi);
  if (i == 2)
    return (info.regs.rdx);
  if (i == 3)
    return (info.regs.rcx);
  if (i == 4)
    return (info.regs.r8);
  if (i == 5)
    return (info.regs.r9);
  return 0;
}

static void	print_string(long reg, int pid)
{
  char		c;
  int		j = 0;

  fprintf(stderr, "\"");
  while ((c = ptrace(PTRACE_PEEKTEXT, pid, reg + j++, NULL))
	 && j < 60)
    fprintf(stderr, (c == '\n') ? "\\n" : "%c", c);
  fprintf(stderr, "\"");
  if (j == 60)
    fprintf(stderr, "...");
}

static int	count_args(long reg, int pid)
{
  int		j = 0;

  while (ptrace(PTRACE_PEEKTEXT, pid, reg + j, NULL))
    j += sizeof(long);
  return j / sizeof(long);
}

static void	print_strtab(long reg, int pid)
{
  char		c;
  long		addr;
  int		nb, k, j = 0;

  if ((nb = count_args(reg, pid)) > 3)
    {    fprintf(stderr, "[/* %d vars */]", nb);      return ;    }
  fprintf(stderr, "[");
  while ((addr = ptrace(PTRACE_PEEKTEXT, pid, reg + j, NULL)))
    {
      fprintf(stderr, "\"");
      k = 0;
      while ((c = ptrace(PTRACE_PEEKTEXT, pid, addr + k++, NULL)) && k < 60)
  	fprintf(stderr, (c == '\n') ? "\\n" : "%c", c);
      fprintf(stderr, "\"");
      if (k == 60)
	fprintf(stderr, "...");
      j += sizeof(long);
      if (ptrace(PTRACE_PEEKTEXT, pid, reg + j, NULL))
	fprintf(stderr, ", ");
    }
  fprintf(stderr, "]");
}

# define MATCH(x) (!strcmp(args[i], x))
void	print_args(const char *call, char **args, struct user infos, int pid)
{
  long	reg;

  for (int i = 0; args[i]; ++i) {
    reg = get_reg(infos, i);
    if (strchr(args[i], '*') && reg == 0)
      fprintf(stderr, "NULL");
    else if (MATCH("char*") ||
	     (!strcmp(call, "write")
	      && (MATCH("void*") || MATCH("void*"))))
      print_string(reg, pid);
    else if (MATCH("int")
	     && (
		 (!strcmp(call, "access") && i == 1) ||
		 (!strcmp(call, "open") && i == 1) ||
		 (!strcmp(call, "mmap") && (i == 2 || i == 3)) ||
		 (!strcmp(call, "mprotect") && i == 2) ||
		 (!strcmp(call, "futex") && i == 1)
		 ))
      int_enum((int)reg, call, i);
    else if (MATCH("int") || MATCH("unsigned int")
	     || MATCH("size_t") || MATCH("pid_t"))
      fprintf(stderr, "%d", (int)reg);
    else if (MATCH("off_t"))
      fprintf(stderr, "%lu", reg);
    else if (MATCH("void*") || MATCH("unsigned long"))
      fprintf(stderr, !reg && !strcmp(call, "mmap") ?
	      "NULL" : "%#lx", reg);
    else if (MATCH("char**") &&
	     (!strcmp(call, "execve")))
      print_strtab(reg, pid);
    else if (strchr(args[i], '*'))
      fprintf(stderr, "%s = %#lx", args[i], reg);
    else
      fprintf(stderr, "%s", args[i]);
    if (args[i+1])
      fprintf(stderr, ", ");
  }
}
