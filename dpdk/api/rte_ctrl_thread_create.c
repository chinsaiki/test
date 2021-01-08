/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <sched.h>
#include <assert.h>
#include <string.h>

//[rongtao@localhost api]$ gcc rte_ctrl_thread_create.c -pthread
//[rongtao@localhost api]$ ./a.out 


#ifndef PATH_MAX
#define PATH_MAX 256
#endif
#define RTE_LOG(MOD, LEVEL, fmt...) printf(fmt)

typedef cpu_set_t rte_cpuset_t;
#define RTE_CPU_AND(dst, src1, src2) CPU_AND(dst, src1, src2)
#define RTE_CPU_OR(dst, src1, src2) CPU_OR(dst, src1, src2)
#define RTE_CPU_FILL(set) do \
{ \
	unsigned int i; \
	CPU_ZERO(set); \
	for (i = 0; i < CPU_SETSIZE; i++) \
		CPU_SET(i, set); \
} while (0)
#define RTE_CPU_NOT(dst, src) do \
{ \
	cpu_set_t tmp; \
	RTE_CPU_FILL(&tmp); \
	CPU_XOR(dst, &tmp, src); \
} while (0)


struct rte_thread_ctrl_params {
	void *(*start_routine)(void *);
	void *arg;
	pthread_barrier_t configured;
};

int rte_thread_setname(pthread_t id, const char *name)
{
	int ret = ENOSYS;
#if defined(__GLIBC__) && defined(__GLIBC_PREREQ)
#if __GLIBC_PREREQ(2, 12)
	ret = pthread_setname_np(id, name);
#endif
#endif
//	RTE_SET_USED(id);
//	RTE_SET_USED(name);
	return -ret;
}

static void *rte_thread_init(void *arg)
{
	int ret;
//	rte_cpuset_t *cpuset = &internal_config.ctrl_cpuset;
	struct rte_thread_ctrl_params *params = arg;
	void *(*start_routine)(void *) = params->start_routine;
	void *routine_arg = params->arg;

	/* Store cpuset in TLS for quick access */
//	memmove(&RTE_PER_LCORE(_cpuset), cpuset, sizeof(rte_cpuset_t));

	ret = pthread_barrier_wait(&params->configured);
	if (ret == PTHREAD_BARRIER_SERIAL_THREAD) {
		pthread_barrier_destroy(&params->configured);
		free(params);
	}

//#ifndef RTE_EXEC_ENV_WINDOWS
//	__rte_trace_mem_per_thread_alloc();
//#endif
	return start_routine(routine_arg);
}

int
rte_ctrl_thread_create(pthread_t *thread, const char *name,
		const pthread_attr_t *attr,
		void *(*start_routine)(void *), void *arg)
{
#ifdef __rtoax__cpuset_
	rte_cpuset_t *cpuset = &internal_config.ctrl_cpuset;
#endif
	struct rte_thread_ctrl_params *params;
	int ret;

	params = malloc(sizeof(*params));
	if (!params)
		return -ENOMEM;

	params->start_routine = start_routine;
	params->arg = arg;

	pthread_barrier_init(&params->configured, NULL, 2);

	ret = pthread_create(thread, attr, rte_thread_init, (void *)params);
	if (ret != 0) {
		free(params);
		return -ret;
	}

	if (name != NULL) {
		ret = rte_thread_setname(*thread, name);
		if (ret < 0)
			RTE_LOG(DEBUG, EAL,
				"Cannot set name for ctrl thread\n");
	}
#ifdef __rtoax__cpuset_
	ret = pthread_setaffinity_np(*thread, sizeof(*cpuset), cpuset);
	if (ret)
		goto fail;
#endif

	ret = pthread_barrier_wait(&params->configured);
	if (ret == PTHREAD_BARRIER_SERIAL_THREAD) {
		pthread_barrier_destroy(&params->configured);
		free(params);
	}

	return 0;

fail:
	if (PTHREAD_BARRIER_SERIAL_THREAD ==
	    pthread_barrier_wait(&params->configured)) {
		pthread_barrier_destroy(&params->configured);
		free(params);
	}
	pthread_cancel(*thread);
	pthread_join(*thread, NULL);
	return -ret;
}

void *task_routine(void *arg)
{
    while(1) sleep(1);
}      

int main()
{
    pthread_t t1, t2;
    rte_ctrl_thread_create(&t1, "t1", NULL, task_routine, NULL);
    rte_ctrl_thread_create(&t2, "t2", NULL, task_routine, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
}
        
