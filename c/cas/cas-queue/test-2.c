/**
 *  test-2.c
 *  
 *  夹带多消息（内存指针）的发送接收
 *  
 *  荣涛  2021年1月13日
 *  
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

#include "cas-queue.h"
#include "bitset.h"

#ifndef TEST_NUM
#define TEST_NUM   (1UL<<22)
#endif
#ifndef MULTI_SEND
#define MULTI_SEND  10
#endif

typedef struct {
    unsigned long que_id;
    int bitmap_maxbits;
    bits_set bitmap_active;
    void *data[BITS_SETSIZE];
}cas_queue_t;

cas_queue_t test_queue = {READY_TO_ENQUEUE, -1, BITS_SET_INITIALIZER, NULL};

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
            if(BITS_ISSET(i%BITS_SETSIZE, &test_queue.bitmap_active)) {
                continue;
            }
            int multi_send = MULTI_SEND;
            while(multi_send > 0 && i < TEST_NUM) {
                if(test_queue.bitmap_maxbits < i%BITS_SETSIZE) {
                    test_queue.bitmap_maxbits = i%BITS_SETSIZE;
                }
                
                test_queue.data[i%BITS_SETSIZE] = (void*)&test_msgs[i];
                BITS_SET(i%BITS_SETSIZE, &test_queue.bitmap_active);
//                printf("Send -> %ld, i = %d\n", test_msgs[i], i);
                i++;
                multi_send--;
            }
            CAS(&test_queue.que_id, READY_TO_ENQUEUE, READY_TO_DEQUEUE);
            if(i >= TEST_NUM) break;
        }
    }
    printf("enqueue exit.\n");
    pthread_exit(NULL);
}

void *dequeue_task(void*arg){

    int i =0, j = 0;
    uint64_t total_msgs = 0;
    uint64_t latency_total = 0;
    unsigned long *pmsg;
    while(1) {
        if(CAS(&test_queue.que_id, READY_TO_DEQUEUE, READY_TO_DEQUEUE)) {

            if(test_queue.bitmap_maxbits < 0) {
                continue;
            }

            for(j=0;j<=test_queue.bitmap_maxbits;j++) {
                if(BITS_ISSET(j, &test_queue.bitmap_active)) {
                    
                    pmsg = (unsigned long*)test_queue.data[j];
//                    printf("Recv -> %ld, i = %d\n", *pmsg, i);
                    BITS_CLR(j, &test_queue.bitmap_active);
                    i++;
                    total_msgs++;
                }
            }
            test_queue.bitmap_maxbits = -1;
            
            latency_total += RDTSC() - latency;
            latency=0;
            CAS(&test_queue.que_id, READY_TO_DEQUEUE, READY_TO_ENQUEUE);
//            printf("recv ok i = %d.\n", i);
            if(i >= TEST_NUM) break;
        }
    }
    printf("dequeue. latency (per ticks %lf, per msgs %lf ns) msgs %ld/%ld.\n", 
            latency_total*1.0/TEST_NUM/MULTI_SEND, 
            latency_total*1.0/TEST_NUM/MULTI_SEND/3000000000*1000000000,
            total_msgs, TEST_NUM);

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


