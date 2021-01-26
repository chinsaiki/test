#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/eventfd.h>

#include "utils.h"

int efd=0;

unsigned long latency = 0;
unsigned long total_latency = 0;
unsigned long total_num = 0;

int rx_irq_init()
{
    int irqfd = eventfd(0, EFD_CLOEXEC);
    return irqfd;
}

void rx_irq_trigger(int irqfd)
{
    int ret = eventfd_write(irqfd, 1);
}


uint64_t rx_irq_wait(int irqfd)
{
    eventfd_t cnt;
    int ret = eventfd_read(irqfd, &cnt);
    return cnt;
}


void *write_task(void*arg){
   
    int i =0, nmsg = 0, ret;
    eventfd_t count = 1;
    
    while(1) {
        ret = eventfd_write(efd, count);
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
        sleep(1);
        ret = eventfd_read(efd, &count);
        if(ret < 0) {
            break;
        }
        log_dequeue("read ret = %d, count = %ld\n", ret, count);
    }
    pthread_exit(NULL);
}

void demo_flags(int inival, int flags)
{
    pthread_t write_taskid, read_taskid;

    
    efd = eventfd(inival, flags);

    pthread_create(&write_taskid, NULL, write_task, NULL);
    pthread_create(&read_taskid, NULL, read_task, NULL);

    pthread_join(write_taskid, NULL);
    pthread_join(read_taskid, NULL);

    printf("Exit.\n");

    close(efd);

}

int main()
{
    demo_flags(0, EFD_CLOEXEC);

//    demo_flags(0, EFD_CLOEXEC|EFD_SEMAPHORE);

//    demo_flags(0, EFD_CLOEXEC|EFD_NONBLOCK);
    
//    demo_flags(0, EFD_CLOEXEC|EFD_NONBLOCK | EFD_SEMAPHORE);
    
    return EXIT_SUCCESS;
}



