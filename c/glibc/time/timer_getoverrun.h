/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */


#include <time.h>

//get overrun count for a POSIX per-process timer
//超时的泛滥次数，正常情况下返回0
int timer_getoverrun(timer_t timerid);

//Link with -lrt.

//On success, timer_getoverrun() returns the overrun count of the specified timer; 
//        this count may be  0  if no overruns have occurred.  
//On failure, -1 is returned, and errno is set to indicate the error.


/*
 * Get the number of overruns of a POSIX.1b interval timer.  This is to
 * be the overrun of the timer last delivered.  At the same time we are
 * accumulating overruns on the next timer.  The overrun is frozen when
 * the signal is delivered, either at the notify time (if the info block
 * is not queued) or at the actual delivery time (as we are informed by
 * the call back to posixtimer_rearm().  So all we need to do is
 * to pick up the frozen overrun.
 */
SYSCALL_DEFINE1(timer_getoverrun, timer_t, timer_id)
{
	struct k_itimer *timr;
	int overrun;
	unsigned long flags;

	timr = lock_timer(timer_id, &flags);
	if (!timr)
		return -EINVAL;

	overrun = timer_overrun_to_int(timr, 0);
	unlock_timer(timr, flags);

	return overrun;
}

