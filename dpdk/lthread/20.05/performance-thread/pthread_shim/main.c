/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2015 Intel Corporation
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <string.h>
#include <sys/queue.h>
#include <stdarg.h>
#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <sched.h>
#include <pthread.h>

#include "lthread_api.h"
#include "lthread_diag_api.h"
#include "pthread_shim.h"


void* test_task_fn(void* unused)
{
	printf("TASK: %ld.\n", pthread_self());

    
    pthread_exit(NULL);
	return NULL;
}
/* The main program. */
int main(int argc, char **argv)
{
	pthread_t t1, t2;
    
	pthread_create(&t1, NULL, test_task_fn, NULL);
	pthread_create(&t2, NULL, test_task_fn, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	return 0;
}

