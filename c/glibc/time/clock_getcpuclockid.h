/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

#include <time.h>

//clock_getcpuclockid - obtain ID of a process CPU-time clock
int clock_getcpuclockid(pid_t pid, clockid_t *clock_id);

//Link with -lrt (only for glibc versions before 2.17).
//return 0-success
//ENOSYS The kernel does not support obtaining the per-process CPU-time clock of another process,  and  pid
//      does not specify the calling process.
//
//EPERM  The  caller does not have permission to access the CPU-time clock of the process specified by pid.
//      (Specified as an optional error in POSIX.1-2001; does not occur on Linux unless  the  kernel  does
//      not support obtaining the per-process CPU-time clock of another process.)
//
//ESRCH  There is no process with the ID pid.