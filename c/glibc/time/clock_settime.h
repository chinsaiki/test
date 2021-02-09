/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

#include <time.h>

struct timespec {
   time_t   tv_sec;        /* seconds */
   long     tv_nsec;       /* nanoseconds */
};

//The  function  clock_getres() finds the resolution (precision) of the specified clock clk_id, and, if res
//       is non-NULL, stores it in the struct timespec pointed to by res.  
int clock_settime(clockid_t clk_id, const struct timespec *tp);


//CLOCK_REALTIME
//      System-wide clock that measures real (i.e., wall-clock) time.  Setting this clock requires  appro‐
//      priate privileges.  This clock is affected by discontinuous jumps in the system time (e.g., if the
//      system administrator manually changes the clock), and by the incremental adjustments performed  by
//      adjtime(3) and NTP.
//
//CLOCK_REALTIME_COARSE (since Linux 2.6.32; Linux-specific)
//      A  faster  but less precise version of CLOCK_REALTIME.  Use when you need very fast, but not fine-
//      grained timestamps.
//
//
//CLOCK_MONOTONIC
//      Clock that cannot be set and represents monotonic  time  since  some  unspecified  starting
//      point.   This clock is not affected by discontinuous jumps in the system time (e.g., if the
//      system administrator manually changes the  clock),  but  is  affected  by  the  incremental
//      adjustments performed by adjtime(3) and NTP.
//
//CLOCK_MONOTONIC_COARSE (since Linux 2.6.32; Linux-specific)
//      A faster but less precise version of CLOCK_MONOTONIC.  Use when you need very fast, but not
//      fine-grained timestamps.
//
//CLOCK_MONOTONIC_RAW (since Linux 2.6.28; Linux-specific)
//      Similar to CLOCK_MONOTONIC, but provides access to a raw hardware-based time  that  is  not
//      subject to NTP adjustments or the incremental adjustments performed by adjtime(3).
//
//CLOCK_BOOTTIME (since Linux 2.6.39; Linux-specific)
//      Identical  to  CLOCK_MONOTONIC,  except  it  also includes any time that the system is sus‐
//      pended.  This allows applications to get a suspend-aware monotonic clock without having  to
//      deal  with  the complications of CLOCK_REALTIME, which may have discontinuities if the time
//      is changed using settimeofday(2).
//
//CLOCK_PROCESS_CPUTIME_ID
//      High-resolution per-process timer from the CPU.
//
//CLOCK_THREAD_CPUTIME_ID
//      Thread-specific CPU-time clock.


/*
 * We preserve minimal support for CLOCK_REALTIME and CLOCK_MONOTONIC
 * as it is easy to remain compatible with little code. CLOCK_BOOTTIME
 * is also included for convenience as at least systemd uses it.
 */

SYSCALL_DEFINE2(clock_settime, const clockid_t, which_clock,
		const struct __kernel_timespec __user *, tp)
{
	struct timespec64 new_tp;

	if (which_clock != CLOCK_REALTIME)
		return -EINVAL;
	if (get_timespec64(&new_tp, tp))
		return -EFAULT;

	return do_sys_settimeofday64(&new_tp, NULL);
}

