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
	printf("TASK Func: %ld.\n", pthread_self());

    int i = 0;
    while(i++ < 2) {
        printf("TASK: %ld -> %d.\n", pthread_self(), i);
    }
    
    pthread_exit(NULL);
	return NULL;
}
/* The main program. */
int main(int argc, char **argv)
{
    
    pthread_override_set(1);
    
    int nr_threads = 4;
    if(argc >= 2) {
        nr_threads = atoi(argv[1])?atoi(argv[1]):4;
    }
    
	pthread_t threadids[nr_threads];
    int i;
    for(i=0;i<nr_threads;i++) {
        pthread_create(&threadids[i], NULL, test_task_fn, NULL);
    }
    for(i=0;i<nr_threads;i++) {
        pthread_join(threadids[i], NULL);
    }

	return 0;
}

