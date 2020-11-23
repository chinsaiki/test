#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <obstack.h>
#include <sys/time.h>

#include "zmalloc.h"

#if 1
#define debug()
#else
#define debug() printf("%s:%d\n",__func__, __LINE__)
#endif

unsigned long int test_alloc_size =  1500;
unsigned long int test_alloc_free_cnt = 9999999UL;




void *mymalloc(ssize_t size){  debug(); return malloc(size);}
void myfree(void*ptr){  debug(); free(ptr);}

unsigned long int diff_timeval(const char *prefix, struct timeval *big, struct timeval *small){
    unsigned long int diffsec = big->tv_sec - small->tv_sec;
    unsigned long int diffusec = diffsec*1000000 + (big->tv_usec - small->tv_usec);
    printf("%s %ld MicroSec\n", prefix, diffusec);
    return diffusec;
}

void test_mem(void*mem, ssize_t size){
//    memset(mem, 0, size);
}

void test_malloc()
{
    struct timeval start, end;
    char *str = NULL;
    
    gettimeofday(&start, NULL);
    unsigned long int i;
    for(i=0;i<test_alloc_free_cnt;i++) {
        str = malloc(test_alloc_size);
        test_mem(str, test_alloc_size);
        free(str);
    }
    
    gettimeofday(&end, NULL);

    diff_timeval(" Malloc>>", &end, &start);
}

#define obstack_chunk_alloc mymalloc
#define obstack_chunk_free myfree
#define obstack_alloc_failed_handler  TODO

void test_obstack()
{
    struct obstack string_obstack;
    char *str = NULL;
    struct timeval start, end;
    
    obstack_init(&string_obstack);
    
    gettimeofday(&start, NULL);
    unsigned long int i;
    for(i=0;i<test_alloc_free_cnt;i++) {
        str = (char*) obstack_alloc (&string_obstack, test_alloc_size);
        test_mem(str, test_alloc_size);
        obstack_free(&string_obstack, str);
    }
    
    gettimeofday(&end, NULL);

    diff_timeval("Obstack>>",  &end, &start);

}


void test_redis_zmalloc()
{
    
    char *str = NULL;
    struct timeval start, end;
        
    gettimeofday(&start, NULL);
    unsigned long int i;
    for(i=0;i<test_alloc_free_cnt;i++) {
        str = (char*) zmalloc (test_alloc_size);
        test_mem(str, test_alloc_size);
        zfree(str);
    }
    
    gettimeofday(&end, NULL);

    diff_timeval("  Redis>>",  &end, &start);
}

int main(int argc, char *argv[])
{
    unsigned long int cnt = 0;
    switch(argc){
        case 3:
            cnt = strtoul(argv[2], NULL, 0);
            test_alloc_size = cnt?cnt:test_alloc_size;

        case 2:
            cnt = strtoul(argv[1], NULL, 0);
            test_alloc_free_cnt = cnt?cnt:test_alloc_free_cnt;

        case 1:
        default:
            printf("Test mem size %u, loop times %u.\n", test_alloc_size, test_alloc_free_cnt);
            break;
    }


    test_malloc();
    test_obstack();
    test_redis_zmalloc();

}

