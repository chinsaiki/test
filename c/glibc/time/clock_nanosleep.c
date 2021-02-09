
#include <time.h>
#include <stdio.h>

#define NS_PER_MS 1000000
#define NS_PER_US 1000
#define NS_PER_SEC 1000000000
#define US_PER_MS 1000
#define US_PER_SEC 1000000
#define MS_PER_SEC 1000

typedef unsigned long long nsec_t;


void nsec_to_ts(nsec_t ns, struct timespec *ts)
{
	if (ts == NULL) {
		/* FIXME: write a real error logging system */
		printf("ERROR in %s: ts is NULL\n", __FUNCTION__);
		return;
	}
	ts->tv_sec = ns / NS_PER_SEC;
	ts->tv_nsec = ns % NS_PER_SEC;
}


void rt_nanosleep(nsec_t ns)
{
	struct timespec ts_sleep, ts_rem;
	int rc;
	nsec_to_ts(ns, &ts_sleep);
	rc = clock_nanosleep(CLOCK_MONOTONIC, 0, &ts_sleep, &ts_rem);
	/* FIXME: when should we display the remainder ? */
	if (rc != 0) {
		printf("WARNING: rt_nanosleep() returned early by %d s %d ns\n",
		       (int)ts_rem.tv_sec, (int)ts_rem.tv_nsec);
	}
}

int main()
{
    int i;

	for(i=0; i<10; i++) {
        rt_nanosleep(1000000000);
		printf("rt_nanosleep\n");
	}
}

