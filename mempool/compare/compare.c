#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "compare.h"

#define REAL_NTHREADS   (get_nr_threads()>=1?get_nr_threads():NR_THREAD)

static unsigned long int test_alloc_size =  1500;
static unsigned long int test_alloc_free_cnt = 9999999UL;
static unsigned int nr_threads = 0;



int set_alloc_size(unsigned long int alloc_size){ test_alloc_size = alloc_size;}
int set_alloc_free_cnt(unsigned long int alloc_free_cnt){  test_alloc_free_cnt = alloc_free_cnt;}
int set_nr_threads(unsigned int number){  nr_threads = number;}

unsigned long int get_alloc_size(){ return test_alloc_size ;}
unsigned long int get_alloc_free_cnt(){ return test_alloc_free_cnt ;}
unsigned long int get_nr_threads(){ return nr_threads ;}

static __attribute__((constructor(101))) void determine_nr_thread()
{
    
}


static unsigned long int diff_timeval_usec(struct timeval *big, struct timeval *small)
{
    unsigned long int diffsec = big->tv_sec - small->tv_sec;
    unsigned long int diffusec = diffsec*1000000 + (big->tv_usec - small->tv_usec);
//    printf("%ld MicroSec\n", diffusec);
    return diffusec;
}

static inline void crypto_method_call_tick(void *arg){}

int malloc_entity_init(struct malloc_entity *entity, const char *name, 
                        method_init initial_fn,
                        method_malloc alloc, 
                        method_test test, 
                        method_free free,
                        void *user_arg)
{
    int ret = 0, i;
    if(!alloc || !free || !test || !initial_fn) {
        fprintf(stderr, "alloc, test and free must non NULL.\n");
        ret = -1;
    }

    strncpy(entity->method_name, name, NAME_LEN);
    entity->initial_fn = initial_fn;
    entity->memory_alloc = alloc;
    entity->memory_test = test;
    entity->memory_free = free;
    entity->method_call_tick = crypto_method_call_tick;
    
    entity->user_arg = user_arg;
    entity->total_diffusec[0] = 0L;
    entity->total_diffusec[1] = 0L;
    entity->total_diffusec[2] = 0L;
    entity->total_diffusec[3] = 0L;
    
    for(i=0;i<REAL_NTHREADS;i++) {
        entity->memory_stats_time[i].diffusec[0] = 0L;
        entity->memory_stats_time[i].diffusec[1] = 0L;
        entity->memory_stats_time[i].diffusec[2] = 0L;
        entity->memory_stats_time[i].diffusec[3] = 0L;

        entity->memory_stats_time[i].blongs_to = entity;
    }
    

    return ret;
}


void *test_routine(void *arg)
{
    struct malloc_stats_time *stats_time = (struct malloc_stats_time*)arg;
    struct malloc_entity *entity = (struct malloc_entity *)(stats_time->blongs_to);

    
    struct timeval start, end;
    char *str = NULL;
    unsigned long int method_call_tick = 0;
    
    unsigned long int i;
    for(i=0;i<test_alloc_free_cnt;i++) {
        
        gettimeofday(&start, NULL);
        str = entity->memory_alloc(entity->user_arg, test_alloc_size);
        gettimeofday(&end, NULL);
        stats_time->diffusec[1] += diff_timeval_usec(&end, &start);
        
        gettimeofday(&start, NULL);
        entity->memory_test(entity->user_arg, str, test_alloc_size);
        gettimeofday(&end, NULL);
        stats_time->diffusec[2] += diff_timeval_usec(&end, &start);
        
        gettimeofday(&start, NULL);
        entity->method_call_tick(NULL);
        gettimeofday(&end, NULL);
//        method_call_tick += diff_timeval_usec(&end, &start);
        method_call_tick = 0;
        
        gettimeofday(&start, NULL);
        entity->memory_free(entity->user_arg, str);
        gettimeofday(&end, NULL);
        stats_time->diffusec[3] += diff_timeval_usec(&end, &start);
    }

    //函数调用的时间
    stats_time->diffusec[1] -= method_call_tick;
    stats_time->diffusec[2] -= method_call_tick;
    stats_time->diffusec[3] -= method_call_tick;

    
    //计算总和
    stats_time->diffusec[0] +=stats_time->diffusec[1] 
                                + stats_time->diffusec[2] 
                                + stats_time->diffusec[3];

    
    
}

void malloc_entity_test(struct malloc_entity *entity)
{
    entity->initial_fn( entity->user_arg);
    

    int ithread;
    for(ithread=0;ithread<REAL_NTHREADS;ithread++) {
        pthread_create(&entity->threadid[ithread], NULL, test_routine, (void*)&entity->memory_stats_time[ithread]);
    }
    for(ithread=0;ithread<REAL_NTHREADS;ithread++) {
        pthread_join(entity->threadid[ithread], NULL);
    }
}


void malloc_entity_display(struct malloc_entity *entity)
{
    static int i =0;
    if(i++==0)
    fprintf(stdout, "%16s %10s %10s %10s %10s\n", "", "Total", "Alloc", "Test", "Free");
    
    int ithread;
    for(ithread=0;ithread<REAL_NTHREADS;ithread++) {
        entity->total_diffusec[0] += entity->memory_stats_time[ithread].diffusec[0];
        entity->total_diffusec[1] += entity->memory_stats_time[ithread].diffusec[1];
        entity->total_diffusec[2] += entity->memory_stats_time[ithread].diffusec[2];
        entity->total_diffusec[3] += entity->memory_stats_time[ithread].diffusec[3];
    }
    fprintf(stdout, "%16s %10ld %10ld %10ld %10ld\n", entity->method_name, 
                        entity->total_diffusec[0], 
                        entity->total_diffusec[1], 
                        entity->total_diffusec[2], 
                        entity->total_diffusec[3]);
}


inline void malloc_common_test(void*ptr, size_t size)
{
    char *s = (char*)ptr;
    int i;
    for(i=0;i<size;i+=10)
        s[i] = 'A'+i%26;
}

