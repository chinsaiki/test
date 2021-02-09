/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>


#define NS_PER_MS 1000000
#define NS_PER_US 1000
#define NS_PER_SEC 1000000000
#define US_PER_MS 1000
#define US_PER_SEC 1000000
#define MS_PER_SEC 1000

typedef unsigned long long nsec_t;


void ts_normalize(struct timespec *ts)
{
	if (ts == NULL) {
		/* FIXME: write a real error logging system */
		printf("ERROR in %s: ts is NULL\n", __FUNCTION__);
		return;
	}

	/* get the abs(nsec) < NS_PER_SEC */
	while (ts->tv_nsec > NS_PER_SEC) {
		ts->tv_sec++;
		ts->tv_nsec -= NS_PER_SEC;
	}
	while (ts->tv_nsec < -NS_PER_SEC) {
		ts->tv_sec--;
		ts->tv_nsec += NS_PER_SEC;
	}

	/* get the values to the same polarity */
	if (ts->tv_sec > 0 && ts->tv_nsec < 0) {
		ts->tv_sec--;
		ts->tv_nsec += NS_PER_SEC;
	}
	if (ts->tv_sec < 0 && ts->tv_nsec > 0) {
		ts->tv_sec++;
		ts->tv_nsec -= NS_PER_SEC;
	}
}

int ts_to_nsec(struct timespec *ts, nsec_t * ns)
{
	struct timespec t;
	if (ts == NULL) {
		/* FIXME: write a real error logging system */
		printf("ERROR in %s: ts is NULL\n", __FUNCTION__);
		return -1;
	}
	t.tv_sec = ts->tv_sec;
	t.tv_nsec = ts->tv_nsec;
	ts_normalize(&t);

	if (t.tv_sec <= 0 && t.tv_nsec < 0) {
		printf("ERROR in %s: ts is negative\n", __FUNCTION__);
		return -1;
	}

	*ns = (nsec_t) ts->tv_sec * NS_PER_SEC + ts->tv_nsec;
	return 0;
}

nsec_t rt_gettime(void)
{
	struct timespec ts;
	nsec_t ns;
	int rc;

	rc = clock_gettime(CLOCK_MONOTONIC, &ts);
	if (rc != 0) {
		printf("ERROR in %s: clock_gettime() returned %d\n",
		       __FUNCTION__, rc);
		perror("clock_gettime() failed");
		return 0;
	}

	ts_to_nsec(&ts, &ns);
	return ns;
}

int main()
{
    int i;

	for(i=0; i<10; i++) {
		printf("%ld ns\n", rt_gettime());
	}
}

