#include <stdio.h>
#include <pthread.h>

#include "cus_spinlock.h"

cus_spinlock_t spinlock;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

#ifndef NR_NUM
#define NR_NUM 50000
#endif
int sum = 0;

void *test_func(void*arg)
{
	int i;
	for(i=0;i<NR_NUM;i++) {
#ifndef NONLOCK
		cus_spinlock_lock(&spinlock);
#endif
		sum+=1;
#ifndef NONLOCK
		cus_spinlock_unlock(&spinlock);
#endif
	}
	pthread_exit(NULL);
}

void *test_func_mutex(void*arg)
{
	int i;
	for(i=0;i<NR_NUM;i++) {
#ifndef NONLOCK
		pthread_mutex_lock(&mutex);
#endif
		sum+=1;
#ifndef NONLOCK
		pthread_mutex_unlock(&mutex);
#endif
	}
	pthread_exit(NULL);
}

int main()
{
	int i, nthreads = 4;
	pthread_t threadids[nthreads];

	cus_spinlock_init(&spinlock);

#ifdef NONLOCK
	printf("Non lock.\n");
#else
#ifdef MUTEX
	printf("Use mutex.\n");
#else
	printf("Use spinlock.\n");
#endif
#endif
	for(i=0;i<nthreads;i++)	{
#ifdef MUTEX
		pthread_create(&threadids[i], NULL, test_func_mutex, NULL);
#else
		pthread_create(&threadids[i], NULL, test_func, NULL);
#endif
	}
	for(i=0;i<nthreads;i++) {
		pthread_join(threadids[i], NULL);
	}
	printf("sum = %d\n", sum);
}
