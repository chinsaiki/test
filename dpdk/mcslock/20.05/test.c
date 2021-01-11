#include <stdio.h>
#include <pthread.h>

#include "rte_mcslock.h"

//gcc test.c -pthread


rte_mcslock_t *pmcslock;

void *task1(void*arg){
    rte_mcslock_t ml;

	rte_mcslock_lock(&pmcslock, &ml);
	printf("MCS lock taken on thread %ld\n", pthread_self());
    sleep(2);
	rte_mcslock_unlock(&pmcslock, &ml);
}

void *task2(void*arg){
    rte_mcslock_t ml;
    
    sleep(1);

	rte_mcslock_lock(&pmcslock, &ml);
	printf("MCS lock taken on thread %ld\n", pthread_self());
	rte_mcslock_unlock(&pmcslock, &ml);

}

int main()
{
    pthread_t t1, t2;

    pthread_create(&t1, NULL, task1, NULL);
    pthread_create(&t2, NULL, task2, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
}
