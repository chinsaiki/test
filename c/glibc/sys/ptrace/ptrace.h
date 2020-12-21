/* `ptrace' debugger support interface.  Linux version.
   Copyright (C) 1996-2012 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef _SYS_PTRACE_H
#define _SYS_PTRACE_H	1

#include <features.h>
#include <bits/types.h>

//__BEGIN_DECLS
    
#define PT_TRACE_ME PTRACE_TRACEME
#define PT_READ_I PTRACE_PEEKTEXT
#define PT_READ_D PTRACE_PEEKDATA
#define PT_READ_U PTRACE_PEEKUSER
#define PT_WRITE_I PTRACE_POKETEXT
#define PT_WRITE_D PTRACE_POKEDATA
#define PT_WRITE_U PTRACE_POKEUSER
#define PT_CONTINUE PTRACE_CONT
#define PT_KILL PTRACE_KILL
#define PT_STEP PTRACE_SINGLESTEP
#define PT_GETREGS PTRACE_GETREGS
#define PT_SETREGS PTRACE_SETREGS
#define PT_GETFPREGS PTRACE_GETFPREGS
#define PT_SETFPREGS PTRACE_SETFPREGS
#define PT_ATTACH PTRACE_ATTACH
#define PT_DETACH PTRACE_DETACH
#define PT_GETFPXREGS PTRACE_GETFPXREGS
#define PT_SETFPXREGS PTRACE_SETFPXREGS
#define PT_SYSCALL PTRACE_SYSCALL
#define PT_SETOPTIONS PTRACE_SETOPTIONS
#define PT_GETEVENTMSG PTRACE_GETEVENTMSG
#define PT_GETSIGINFO PTRACE_GETSIGINFO
#define PT_SETSIGINFO PTRACE_SETSIGINFO
#define PTRACE_GETREGSET PTRACE_GETREGSET
#define PTRACE_SETREGSET PTRACE_SETREGSET
#define PTRACE_SEIZE PTRACE_SEIZE
#define PTRACE_INTERRUPT PTRACE_INTERRUPT
#define PTRACE_LISTEN PTRACE_LISTEN
#define PTRACE_PEEKSIGINFO PTRACE_PEEKSIGINFO
#define PTRACE_GETSIGMASK PTRACE_GETSIGMASK
#define PTRACE_SETSIGMASK PTRACE_SETSIGMASK
#define PTRACE_SECCOMP_GET_FILTER PTRACE_SECCOMP_GET_FILTER

/* Type of the REQUEST argument to `ptrace.'  */
enum __ptrace_request
{
  /* Indicate that the process making this request should be traced.
     All signals received by this process can be intercepted by its
     parent, and its parent can use the other `ptrace' requests.  */
  PTRACE_TRACEME = 0,

  /* Return the word in the process's text space at address ADDR.  */
  PTRACE_PEEKTEXT = 1,

  /* Return the word in the process's data space at address ADDR.  */
  PTRACE_PEEKDATA = 2,

  /* Return the word in the process's user area at offset ADDR.  */
  PTRACE_PEEKUSER = 3, /* peek: 窥探，偷看 */

  /* Write the word DATA into the process's text space at address ADDR.  */
  PTRACE_POKETEXT = 4,

  /* Write the word DATA into the process's data space at address ADDR.  */
  PTRACE_POKEDATA = 5,

  /* Write the word DATA into the process's user area at offset ADDR.  */
  PTRACE_POKEUSER = 6, /* poke: 戳，伸出，捅 */

  /* Continue the process.  */
  PTRACE_CONT = 7,

  /* Kill the process.  */
  PTRACE_KILL = 8,

  /* Single step the process. 使用PTRACE_SINGLESTEP计算进程的机器指令
     This is not supported on all machines.  */
  PTRACE_SINGLESTEP = 9,

  /* Get all general purpose registers used by a processes.
     This is not supported on all machines.  */
   PTRACE_GETREGS = 12,

  /* Set all general purpose registers used by a processes.
     This is not supported on all machines.  */
   PTRACE_SETREGS = 13,

  /* Get all floating point registers used by a processes.
     This is not supported on all machines.  */
   PTRACE_GETFPREGS = 14,

  /* Set all floating point registers used by a processes.
     This is not supported on all machines.  */
   PTRACE_SETFPREGS = 15,

  /* Attach to a process that is already running. */
  PTRACE_ATTACH = 16,

  /* Detach from a process attached to with PTRACE_ATTACH.  */
  PTRACE_DETACH = 17,

  /* Get all extended floating point registers used by a processes.
     This is not supported on all machines.  */
   PTRACE_GETFPXREGS = 18,

  /* Set all extended floating point registers used by a processes.
     This is not supported on all machines.  */
   PTRACE_SETFPXREGS = 19,

  /* Continue and stop at the next (return from) syscall.  */
  PTRACE_SYSCALL = 24,

  /* Set ptrace filter options.  */
  PTRACE_SETOPTIONS = 0x4200,

  /* Get last ptrace message.  */
  PTRACE_GETEVENTMSG = 0x4201,

  /* Get siginfo for process.  */
  PTRACE_GETSIGINFO = 0x4202,

  /* Set new siginfo for process.  */
  PTRACE_SETSIGINFO = 0x4203,

  /* Get register content.  */
  PTRACE_GETREGSET = 0x4204,

  /* Set register content.  */
  PTRACE_SETREGSET = 0x4205,

  /* Like PTRACE_ATTACH, but do not force tracee to trap and do not affect
     signal or group stop state.  */
  PTRACE_SEIZE = 0x4206,

  /* Trap seized tracee.  */
  PTRACE_INTERRUPT = 0x4207,

  /* Wait for next group event.  */
  PTRACE_LISTEN = 0x4208,

  PTRACE_PEEKSIGINFO = 0x4209,

  PTRACE_GETSIGMASK = 0x420a,

  PTRACE_SETSIGMASK = 0x420b,

  PTRACE_SECCOMP_GET_FILTER = 0x420c
};


/* Flag for PTRACE_LISTEN.  */
enum __ptrace_flags
{
  PTRACE_SEIZE_DEVEL = 0x80000000
};

/* Options set using PTRACE_SETOPTIONS.  */
enum __ptrace_setoptions
{
  PTRACE_O_TRACESYSGOOD	= 0x00000001,
  PTRACE_O_TRACEFORK	= 0x00000002,
  PTRACE_O_TRACEVFORK   = 0x00000004,
  PTRACE_O_TRACECLONE	= 0x00000008,
  PTRACE_O_TRACEEXEC	= 0x00000010,
  PTRACE_O_TRACEVFORKDONE = 0x00000020,
  PTRACE_O_TRACEEXIT	= 0x00000040,
  PTRACE_O_TRACESECCOMP = 0x00000080,
  PTRACE_O_MASK		= 0x000000ff
};

/* Wait extended result codes for the above trace options.  */
enum __ptrace_eventcodes
{
  PTRACE_EVENT_FORK	= 1,
  PTRACE_EVENT_VFORK	= 2,
  PTRACE_EVENT_CLONE	= 3,
  PTRACE_EVENT_EXEC	= 4,
  PTRACE_EVENT_VFORK_DONE = 5,
  PTRACE_EVENT_EXIT	= 6,
  PTRAVE_EVENT_SECCOMP  = 7
};

/* Arguments for PTRACE_PEEKSIGINFO.  */
struct __ptrace_peeksiginfo_args
{
  __uint64_t off;	/* From which siginfo to start.  */
  __uint32_t flags;	/* Flags for peeksiginfo.  */
  __int32_t nr;		/* How many siginfos to take.  */
};

enum __ptrace_peeksiginfo_flags
{
  /* Read signals from a shared (process wide) queue.  */
  PTRACE_PEEKSIGINFO_SHARED = (1 << 0)
};

/* Perform process tracing functions.  REQUEST is one of the values
   above, and determines the action to be taken.
   For all requests except PTRACE_TRACEME, PID specifies the process to be
   traced.

   PID and the other arguments described above for the various requests should
   appear (those that are used for the particular request) as:
     pid_t PID, void *ADDR, int DATA, void *ADDR2
   after REQUEST.  */
extern long int ptrace (enum __ptrace_request __request, ...) __THROW;

//__END_DECLS

#endif /* _SYS_PTRACE_H */
