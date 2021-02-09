/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */


#include <time.h>

int timer_delete(timer_t timerid);

//Link with -lrt.

/* Delete a POSIX.1b interval timer. */
SYSCALL_DEFINE1(timer_delete, timer_t, timer_id)
{
	struct k_itimer *timer;
	unsigned long flags;

	timer = lock_timer(timer_id, &flags);

retry_delete:
	if (!timer)
		return -EINVAL;

	if (unlikely(timer_delete_hook(timer) == TIMER_RETRY)) {
		/* Unlocks and relocks the timer if it still exists */
		timer = timer_wait_running(timer, &flags);
		goto retry_delete;
	}

	spin_lock(&current->sighand->siglock);
	list_del(&timer->list);
	spin_unlock(&current->sighand->siglock);
	/*
	 * This keeps any tasks waiting on the spin lock from thinking
	 * they got something (see the lock code above).
	 */
	timer->it_signal = NULL;

	unlock_timer(timer, flags);
	release_posix_timer(timer, IT_ID_SET);
	return 0;
}

