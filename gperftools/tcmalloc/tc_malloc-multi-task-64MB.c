#include <gperftools/tcmalloc.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
    
void* tc_malloc(size_t size);
void tc_free(void* ptr);

size_t tc_malloc_size(void* ptr);

void* task_routine(void *arg)
{
#define ALLOC_SIZE	1024*1024*64
    char *str = tc_malloc(ALLOC_SIZE);
	memset(str, 0, ALLOC_SIZE);
	int i;
	for (i=0;i<ALLOC_SIZE;i++) {
		str[i] = 'A';	
	}

    printf("%d MB\n", tc_malloc_size(str)/1024/1024);

    tc_free(str);

	pthread_exit(NULL);
}

int main()
{
	int itask;
	pthread_t tasks[10];
	for(itask=0;itask<10;itask++) {
		pthread_create(&tasks[itask], NULL, task_routine, NULL);
	}
	for(itask=0;itask<10;itask++) {
		pthread_join(tasks[itask], NULL);
	}
}
