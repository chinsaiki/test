#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compare.h"



static unsigned long int test_alloc_size =  1500;
static unsigned long int test_alloc_free_cnt = 9999999UL;



int set_alloc_size(unsigned long int alloc_size){ test_alloc_size = alloc_size;}
int set_alloc_free_cnt(unsigned long int alloc_free_cnt){  test_alloc_free_cnt = alloc_free_cnt;}
unsigned long int get_alloc_size(){ return test_alloc_size ;}
unsigned long int get_alloc_free_cnt(){ return test_alloc_free_cnt ;}



static unsigned long int diff_timeval_usec(struct timeval *big, struct timeval *small)
{
    unsigned long int diffsec = big->tv_sec - small->tv_sec;
    unsigned long int diffusec = diffsec*1000000 + (big->tv_usec - small->tv_usec);
//    printf("%ld MicroSec\n", diffusec);
    return diffusec;
}

static void crypto_method_call_tick(void *arg){}

int malloc_entity_init(struct malloc_entity *entity, const char *name, 
                        method_init initial_fn,
                        method_malloc alloc, 
                        method_test test, 
                        method_free free)
{
    int ret = 0;
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
    entity->diffusec[0] = 0L;
    entity->diffusec[1] = 0L;
    entity->diffusec[2] = 0L;
    entity->diffusec[3] = 0L;

    return ret;
}


void malloc_entity_test(struct malloc_entity *entity)
{
    struct timeval start, end;
    char *str = NULL;
    unsigned long int method_call_tick = 0;
    entity->initial_fn();
    
    unsigned long int i;
    for(i=0;i<test_alloc_free_cnt;i++) {
        
        gettimeofday(&start, NULL);
        str = entity->memory_alloc(test_alloc_size);
        gettimeofday(&end, NULL);
        entity->diffusec[1] += diff_timeval_usec(&end, &start);
        
        gettimeofday(&start, NULL);
        entity->memory_test(str, test_alloc_size);
        gettimeofday(&end, NULL);
        entity->diffusec[2] += diff_timeval_usec(&end, &start);
        
        gettimeofday(&start, NULL);
        entity->method_call_tick(NULL);
        gettimeofday(&end, NULL);
        method_call_tick += diff_timeval_usec(&end, &start);
        
        gettimeofday(&start, NULL);
        entity->memory_free(str);
        gettimeofday(&end, NULL);
        entity->diffusec[3] += diff_timeval_usec(&end, &start);
    }

    //函数调用的时间
    entity->diffusec[1] -= method_call_tick;
    entity->diffusec[2] -= method_call_tick;
    entity->diffusec[3] -= method_call_tick;

    
    
    entity->diffusec[0] +=entity->diffusec[1] + entity->diffusec[2] +entity->diffusec[3];

    
}


void malloc_entity_display(struct malloc_entity *entity)
{
    static int i =0;
    if(i++==0)
    fprintf(stderr, "%16s %10s %10s %10s %10s\n", "", "Total", "Alloc", "Test", "Free");
    fprintf(stderr, "%16s %10ld %10ld %10ld %10ld\n", entity->method_name, 
        entity->diffusec[0], entity->diffusec[1], entity->diffusec[2], entity->diffusec[3]);
}


inline void malloc_common_test(void*ptr, size_t size)
{
    char *s = (char*)ptr;
    int i;
    for(i=0;i<size;i+=10)
        s[i] = 'A'+i%26;
}

