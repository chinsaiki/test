#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/eventfd.h>

#include "utils.h"

int efd=0, efd2 = 0;

unsigned long latency = 0;
unsigned long total_latency = 0;
unsigned long total_num = 0;

void *write_task(void*arg){
   
    int i =0, nmsg = 0, ret;
    eventfd_t count = 1;
    
    ret = eventfd_read(efd, &count);
    
    while(1) {
        ret = eventfd_read(efd2, &count);
        latency = RDTSC();
//        printf(">>>1\n");
//        sleep(1);
        ret = eventfd_write(efd, 1);
        if(ret < 0) {
            continue;
        }
    }
    pthread_exit(NULL);
}

void *read_task(void*arg){

    int i =0;
    int ret;
    eventfd_t count;
    
    while(1) {
        ret = eventfd_read(efd, &count);
        if(ret < 0) {
            log_dequeue("read error ret = %d, count = %ld\n", ret, count);
            break;
        }
//        printf(">>>2\n");
        total_latency += RDTSC() - latency;
        total_num ++;
        
        if(total_num % 100000 == 0) {
            printf("latency. ticks %lf, per msgs \033[1;31m%lf ns\033[m, msgs (total %ld).\n", 
                total_latency*1.0/total_num, 
                total_latency*1.0/total_num/3000000000*1000000000, total_num);
        }
        ret = eventfd_write(efd2, 1);
    }
    pthread_exit(NULL);
}

void demo_flags(int inival, int flags)
{
    pthread_t write_taskid, read_taskid;

    
    efd = eventfd(1, flags);
    efd2 = eventfd(1, flags);

    pthread_create(&write_taskid, NULL, write_task, NULL);
    pthread_create(&read_taskid, NULL, read_task, NULL);

    pthread_join(write_taskid, NULL);
    pthread_join(read_taskid, NULL);

    printf("Exit.\n");

    close(efd);
    close(efd2);

}

int main()
{
    printf("test EFD_CLOEXEC|EFD_SEMAPHORE\n");
    demo_flags(0, EFD_CLOEXEC|EFD_SEMAPHORE);

    return EXIT_SUCCESS;
}



