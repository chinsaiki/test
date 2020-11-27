#ifndef __compare_h
#define __compare_h 1

#include <assert.h>
#include <sys/time.h>
#include <sys/types.h>

#ifndef NR_THREAD
#define NR_THREAD 1
#endif

#define NAME_LEN    32

#ifdef DEBUG
#define debug() printf("%s:%d\n",__func__, __LINE__)
#define debug_print(fmt...) do{printf("%s:%d ",__func__, __LINE__); printf(fmt)}while(0)
#else
#define debug() 
#define debug_print(fmt...)
#endif


typedef void (*method_init)(void *user_arg);

typedef void* (*method_malloc)(void *user_arg, size_t size);
typedef void (*method_test)(void *user_arg, void*ptr, size_t size);
typedef void (*method_free)(void *user_arg, void*ptr);

struct malloc_entity;

struct malloc_stats_time{
    struct malloc_entity *blongs_to;
    unsigned long int diffusec[4]; //0-total, 1-alloc, 2-test, 3-free
};

struct malloc_entity{
    char method_name[NAME_LEN];

    void *user_arg;
    
    method_init initial_fn;
    method_malloc memory_alloc;
    method_test memory_test;
    method_free memory_free;

    void (*method_call_tick)(void *arg); //用于矫正函数调用花费的时间
    
    pthread_t threadid[NR_THREAD];
    struct malloc_stats_time memory_stats_time[NR_THREAD];
    unsigned long int total_diffusec[4]; //0-total, 1-alloc, 2-test, 3-free
};


int set_alloc_size(unsigned long int alloc_size);
int set_alloc_free_cnt(unsigned long int alloc_free_cnt);

unsigned long int get_alloc_size();
unsigned long int get_alloc_free_cnt();

int malloc_entity_init(struct malloc_entity *entity, const char *name, method_init initial_fn,
                            method_malloc alloc, method_test test, method_free free, void *user_arg);

void malloc_entity_test(struct malloc_entity *entity);
void malloc_entity_display(struct malloc_entity *entity);

inline void malloc_common_test(void*ptr, size_t size);



#endif /*<__compare_h>*/
