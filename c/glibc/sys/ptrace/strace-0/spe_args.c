
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include "strace.h"
#include "enums.h"

static void	enum_access(int val)
{
  char		flag = 0;

  if (val == F_OK)
    {
      fprintf(stderr, "F_OK");
      return ;
    }
  for (int i = 0; access_flags[i].str; ++i)
    if (access_flags[i].val & val)
      {
	fprintf(stderr, (flag ? "|%s" : "%s"), access_flags[i].str);
	flag = 1;
      }
}

static void	enum_open(int val)
{
  char		flag = 0;

  if (val & O_RDWR)
    {
      fprintf(stderr, "O_RDWR");
      flag = 1;
    }
  else if (val & O_WRONLY)
    {
      fprintf(stderr, "O_WRONLY");
      flag = 1;
    }
  else /* O_RDONLY */
    {
      fprintf(stderr, "O_RDONLY");
      flag = 1;
    }
  for (int i = 0; open_flags[i].str; ++i)
    if (open_flags[i].val & val)
      {
	fprintf(stderr, (flag ? "|%s" : "%s"), open_flags[i].str);
	flag = 1;
      }
}

void		enum_mmap_prot(int val)
{
  char		flag = 0;

  if (val == PROT_NONE)
    {
      fprintf(stderr, "PROT_NONE");
      return ;
    }
  for (int i = 0; mmap_prot[i].str; ++i)
    if (mmap_prot[i].val & val)
      {
	fprintf(stderr, (flag ? "|%s" : "%s"), mmap_prot[i].str);
	flag = 1;
      }
}

void		enum_mmap_flags(int val)
{
  char		flag = 0;

  for (int i = 0; mmap_flags[i].str; ++i)
    if (mmap_flags[i].val & val)
      {
	fprintf(stderr, (flag ? "|%s" : "%s"), mmap_flags[i].str);
	flag = 1;
      }
}

void		enum_futex_op(int val)
{
  char		flag = 0;

  for (int i = 0; futex_op[i].str; ++i)
    if (futex_op[i].val & val)
      {
	fprintf(stderr, (flag ? "|%s" : "%s"), futex_op[i].str);
	flag = 1;
      }
}

#define MATCH(x)	(!strcmp(call, x))
void		int_enum(int value, const char* call, int i)
{
  if (MATCH("access"))
    enum_access(value);
  else if (MATCH("open"))
    enum_open(value);
  else if ((MATCH("mmap") || MATCH("mprotect")) && i == 2)
    enum_mmap_prot(value);
  else if (MATCH("mmap") && i == 3)
    enum_mmap_flags(value);
  else if (MATCH("futex"))
    enum_futex_op(value);
}
