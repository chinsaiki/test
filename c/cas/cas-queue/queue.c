#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

#include "cas.h"

#define TEST_NUM    (1UL<<20)

typedef enum  {
    READY_TO_ENQUEUE = 1,
    READY_TO_DEQUEUE = 2,
}QUEUE_OPS;

typedef unsigned long queue_t;

queue_t test_queue = READY_TO_ENQUEUE;

uint64_t latency;

#define cas_queue_can_enqueue(queue) \
    CAS(&queue, READY_TO_ENQUEUE, READY_TO_ENQUEUE)
    
#define cas_queue_ready_enqueue(queue) \
    CAS(&queue, READY_TO_DEQUEUE, READY_TO_ENQUEUE)
    
#define cas_sendto_queue(queue) \
    CAS(&queue, READY_TO_ENQUEUE, READY_TO_DEQUEUE)

#define cas_recvfrom_queue(queue) \
    CAS(&queue, READY_TO_DEQUEUE, READY_TO_DEQUEUE)

void *enqueue_task(void*arg){

    int i =0;
    while(1) {
        if(cas_queue_can_enqueue(test_queue)) {

            latency = RDTSC();
            
            cas_sendto_queue(test_queue);
            
            if(++i == TEST_NUM) break;
        }
    }

    pthread_exit(NULL);
}

void *dequeue_task(void*arg){

    int i =0;
    
    uint64_t latency_total = 0;
    
    while(1) {
        if(cas_recvfrom_queue(test_queue)) {
            
            latency_total += RDTSC() - latency;
                        
            latency=0;
            
            cas_queue_ready_enqueue(test_queue);
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
