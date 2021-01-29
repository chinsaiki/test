
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <linux/futex.h>
#include "strace.h"

typedef struct	s_flags
{
  int		val;
  char		*str;
}		t_flags;

t_flags		access_flags[] =
  {
    {R_OK, "R_OK"},
    {W_OK, "W_OK"},
    {X_OK, "X_OK"},
    {0, NULL}
  };

t_flags		open_flags[] =
  {
    {O_APPEND, "O_APPEND"},
    {O_ASYNC, "O_ASYNC"},
    {O_CLOEXEC, "O_CLOEXEC"},
    {O_CREAT, "O_CREAT"},
    {O_DIRECT, "O_DIRECT"},
    {O_DIRECTORY, "O_DIRECTORY"},
    {O_EXCL, "O_EXCL"},
    {O_LARGEFILE, "O_LARGEFILE"},
    {O_NOATIME, "O_NOATIME"},
    {O_NOCTTY, "O_NOCTTY"},
    {O_NOFOLLOW, "O_NOFOLLOW"},
    {O_NONBLOCK, "O_NONBLOCK"},
    {O_SYNC, "O_SYNC"},
    {O_TRUNC, "O_TRUNC"},
    {0, NULL}
  };

t_flags		mmap_prot[] =
  {
    {PROT_EXEC, "PROT_EXEC"},
    {PROT_READ, "PROT_READ"},
    {PROT_WRITE, "PROT_WRITE"},
    {0, NULL}
  };

t_flags		mmap_flags[] =
  {
    {MAP_SHARED, "MAP_SHARED"},
    {MAP_PRIVATE, "MAP_PRIVATE"},
    {MAP_32BIT, "MAP_32BIT"},
    {MAP_ANONYMOUS, "MAP_ANONYMOUS"},
    {MAP_DENYWRITE, "MAP_DENYWRITE"},
    {MAP_FIXED, "MAP_FIXED"},
    {MAP_GROWSDOWN, "MAP_GROWSDOWN"},
    {MAP_HUGETLB, "MAP_HUGETLB"},
    {MAP_LOCKED, "MAP_LOCKED"},
    {MAP_NONBLOCK, "MAP_NONBLOCK"},
    {MAP_NORESERVE, "MAP_NORESERVE"},
    {MAP_POPULATE, "MAP_POPULATE"},
    {MAP_STACK, "MAP_STACK"},
    {0, NULL}
  };

t_flags		futex_op[] =
  {
    {FUTEX_WAIT, "FUTEX_WAIT"},
    {FUTEX_WAKE, "FUTEX_WAKE"},
    {FUTEX_FD, "FUTEX_FD"},
    {FUTEX_REQUEUE, "FUTEX_REQUEUE"},
    {FUTEX_CMP_REQUEUE, "FUTEX_CMP_REQUEUE"},
    {0, NULL},
  };
