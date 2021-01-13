/**
 *  test-1.c
 *  
 *  夹带消息（内存指针）的发送接收
 *  
 *  荣涛  2021年1月13日
 *  
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

#include "cas-queue.h"

#define TEST_NUM   (1UL<<20)

static struct {
    unsigned long que_id;
    void *data;
}test_queue = {READY_TO_ENQUEUE, NULL};

uint64_t latency;

void *enqueue_task(void*arg){
    int i =0;
    static unsigned long test_msgs[TEST_NUM] = {0UL};
    for(i=0;i<TEST_NUM;i++) {
        test_msgs[i] = i+1;
    }
    i=0;
    while(1) {
        if(CAS(&test_queue.que_id, READY_TO_ENQUEUE, READY_TO_ENQUEUE)) {
            latency = RDTSC();
            test_queue.data = (void*)&test_msgs[i];
            
            CAS(&test_queue.que_id, READY_TO_ENQUEUE, READY_TO_DEQUEUE);
            if(++i == TEST_NUM) break;
        }
    }
    pthread_exit(NULL);
}

void *dequeue_task(void*arg){

    int i =0;
    uint64_t latency_total = 0;
    unsigned long *pmsg;
    while(1) {
        if(CAS(&test_queue.que_id, READY_TO_DEQUEUE, READY_TO_DEQUEUE)) {

            pmsg = (unsigned long*)test_queue.data;
//            printf("%ld\n", *pmsg);
            latency_total += RDTSC() - latency;
            latency=0;
            CAS(&test_queue.que_id, READY_TO_DEQUEUE, READY_TO_ENQUEUE);
            if(++i == TEST_NUM) break;
        }
    }
    printf("dequeue. latency ticks = %lf(%lf ns)\n", 
            latency_total*1.0/TEST_NUM, 
            latency_total*1.0/TEST_NUM/3000000000*1000000000);

    pthread_exit(NULL);
}



int main()
{
    pthread_t enqueue_taskid, dequeue_taskid;

    pthread_create(&enqueue_taskid, NULL, enqueue_task, NULL);
    pthread_create(&dequeue_taskid, NULL, dequeue_task, NULL);

    pthread_join(enqueue_taskid, NULL);
    pthread_join(dequeue_taskid, NULL);

    return EXIT_SUCCESS;
}

