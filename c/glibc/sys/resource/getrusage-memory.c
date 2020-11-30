#include <stdio.h>
#include <sys/resource.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


static void demo1() 
{
    char *str = malloc(1024*1024*1024);
    str[1023] = 'A';
    
    struct rusage r_usage;
    getrusage(RUSAGE_SELF,&r_usage);
    // Print the maximum resident set size used (in kilobytes).
    printf("Memory usage: %ld kilobytes\n",r_usage.ru_maxrss);
    
    free(str);
}


static void demo2() 
{
    char *str = malloc(1024*1024*1024);
    str[1023] = 'A';
    
    errno = 0;
    struct rusage memory;
    getrusage(RUSAGE_SELF, &memory);
    if(errno == EFAULT)
        printf("Error: EFAULT\n");
    else if(errno == EINVAL)
        printf("Error: EINVAL\n");

    
    
    printf("Usage: %ld\n", memory.ru_ixrss);
    printf("Usage: %ld\n", memory.ru_isrss);
    printf("Usage: %ld\n", memory.ru_idrss);
    printf("Max: %ld\n", memory.ru_maxrss);
    
    free(str);
}

/*
 * Measures the current (and peak) resident and virtual memories
 * usage of your linux C process, in kB
 */
void getMemory(long int* currRealMem, long int* peakRealMem, long int* currVirtMem, long int* peakVirtMem) 
{
    // stores each word in status file
    char buffer[1024] = "";

    // linux file contains this-process info
    FILE* file = fopen("/proc/self/status", "r");

    // read the entire file
    while (fscanf(file, " %1023s", buffer) == 1) {

        if (strcmp(buffer, "VmRSS:") == 0) {
            fscanf(file, " %ld", currRealMem);
        }
        if (strcmp(buffer, "VmHWM:") == 0) {
            fscanf(file, " %ld", peakRealMem);
        }
        if (strcmp(buffer, "VmSize:") == 0) {
            fscanf(file, " %ld", currVirtMem);
        }
        if (strcmp(buffer, "VmPeak:") == 0) {
            fscanf(file, " %ld", peakVirtMem);
        }
    }
    fclose(file);
}



static void demo3() 
{
    long int currRealMem, peakRealMem, currVirtMem, peakVirtMem;

    char *str = malloc(1024*1024*1024);
    str[1024*1024-1] = 'A';
    
    getMemory(&currRealMem, &peakRealMem, &currVirtMem, &peakVirtMem);

    printf("%ld, %ld, %ld, %ld\n", currRealMem, peakRealMem, currVirtMem, peakVirtMem);

    free(str);
}

struct thread_mm_ops{
    int flag;
    
};

void *thread_task_fn(void*arg)
{
    struct thread_mm_ops *ops = (struct thread_mm_ops*)arg;
    
    long int currRealMem, peakRealMem, currVirtMem, peakVirtMem;
    char *str;
    
    if(ops->flag) {
        str = malloc(1024*1024*1024);
        str[1024*1024-1] = 'A';
    }
    
    /* 这里获取的内存使用 情况好像为整个进程的情况 */
    getMemory(&currRealMem, &peakRealMem, &currVirtMem, &peakVirtMem);

    printf("%ld, %ld, %ld, %ld\n", currRealMem, peakRealMem, currVirtMem, peakVirtMem);

    if(ops->flag) {
        free(str);
    }
    
    pthread_exit(NULL);
}

static void demo4_multi_thread() 
{
    int ithread, nthread;

    nthread = 4;

    pthread_t threadids[nthread];
    struct thread_mm_ops threadopss[4] = {
        {0},{1},{0},{1},
    };

    

    for(ithread=0;ithread<nthread;ithread++){
        pthread_create(&threadids[ithread], NULL, thread_task_fn, &threadopss[ithread]);
    }
    for(ithread=0;ithread<nthread;ithread++){
        pthread_join(threadids[ithread], NULL);
    }
    
}


int main() 
{
//    demo1();
    demo2();
//    demo3();
//    demo4_multi_thread();
}
