
enum {
	NAPI_STATE_SCHED,	/* Poll is scheduled */
	NAPI_STATE_MISSED,	/* reschedule a napi */
	NAPI_STATE_DISABLE,	/* Disable pending */
	NAPI_STATE_NPSVC,	/* Netpoll - don't dequeue from poll_list */
	NAPI_STATE_HASHED,	/* In NAPI hash (busy polling possible) */
	NAPI_STATE_NO_BUSY_POLL,/* Do not add in napi_hash, no busy polling */
	NAPI_STATE_IN_BUSY_POLL,/* sk_busy_loop() owns this NAPI */
};

enum {
	NAPIF_STATE_SCHED	 = BIT(NAPI_STATE_SCHED),
	NAPIF_STATE_MISSED	 = BIT(NAPI_STATE_MISSED),
	NAPIF_STATE_DISABLE	 = BIT(NAPI_STATE_DISABLE),
	NAPIF_STATE_NPSVC	 = BIT(NAPI_STATE_NPSVC),
	NAPIF_STATE_HASHED	 = BIT(NAPI_STATE_HASHED),
	NAPIF_STATE_NO_BUSY_POLL = BIT(NAPI_STATE_NO_BUSY_POLL),
	NAPIF_STATE_IN_BUSY_POLL = BIT(NAPI_STATE_IN_BUSY_POLL),
};

/**
 *	napi_schedule_prep - check if napi can be scheduled
 *	@n: napi context
 *
 * Test if NAPI routine is already running, and if not mark
 * it as running.  This is used as a condition variable
 * insure only one NAPI poll instance runs.  We also make
 * sure there is no pending NAPI disable.
 */
bool napi_schedule_prep(struct napi_struct *n)
{
	unsigned long val, new;

	do {
		val = READ_ONCE(n->state);
		if (unlikely(val & NAPIF_STATE_DISABLE))
			return false;
		new = val | NAPIF_STATE_SCHED;

		/* Sets STATE_MISSED bit if STATE_SCHED was already set
		 * This was suggested by Alexander Duyck, as compiler
		 * emits better code than :
		 * if (val & NAPIF_STATE_SCHED)
		 *     new |= NAPIF_STATE_MISSED;
		 */
		new |= (val & NAPIF_STATE_SCHED) / NAPIF_STATE_SCHED *
						   NAPIF_STATE_MISSED;
	} while (cmpxchg(&n->state, val, new) != val);

	return !(val & NAPIF_STATE_SCHED);
}
EXPORT_SYMBOL(napi_schedule_prep);

