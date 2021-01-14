/**
 *  test-2.c
 *  
 *  夹带多消息（内存指针）的发送接收
 *  
 *  荣涛  2021年1月13日
 *  荣涛2021年1月14日  添加绑核
 *  
 */
#define _GNU_SOURCE
#include <sched.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

#include "cas-queue.h"
#include "bitset.h"
#include "log.h"

#ifndef TEST_NUM
#define TEST_NUM   (1UL<<20)
#endif
#ifndef MULTI_SEND
#define MULTI_SEND  10
#endif

typedef struct cas_queue_s {
    unsigned long que_id;
    int bitmap_maxbits;
    bits_set CACHE_ALIGNED bitmap_active;
    void CACHE_ALIGNED * data[BITS_SETSIZE];
}CACHE_ALIGNED cas_queue_t;

cas_queue_t test_queue = {READY_TO_ENQUEUE, -1, BITS_SET_INITIALIZER, NULL};

uint64_t latency;

typedef struct test_msgs_s {
#define TEST_MSG_MAGIC 0x123123ff    
    int magic;
    unsigned long value;
}test_msgs_t;

static inline int set_cpu_affinity(int i)
{
    cpu_set_t mask;
    CPU_ZERO(&mask);
 
    CPU_SET(i,&mask);
 
    printf("setaffinity thread %u(tid %d) to CPU%d\n", pthread_self(), gettid(), i);
    if(-1 == pthread_setaffinity_np(pthread_self() ,sizeof(mask),&mask)) {
        printf("pthread_setaffinity_np error.\n");
        return -1;
    }
    return 0;
}


void *enqueue_task(void*arg){

    set_cpu_affinity(0);
    
    int i =0;
    test_msgs_t *pmsgs = (test_msgs_t *)arg;
    while(1) {
        if(CAS(&test_queue.que_id, READY_TO_ENQUEUE, ENQUEUING)) {
            
            latency = RDTSC();
            
            if(BITS_ISSET(i%BITS_SETSIZE, &test_queue.bitmap_active)) {
                continue;
            }
            int multi_send = MULTI_SEND;
            while(multi_send > 0 && i < TEST_NUM) {
                if(test_queue.bitmap_maxbits < i%BITS_SETSIZE) {
                    test_queue.bitmap_maxbits = i%BITS_SETSIZE;
                }
                
                test_queue.data[i%BITS_SETSIZE] = (void*)&pmsgs[i];
                BITS_SET(i%BITS_SETSIZE, &test_queue.bitmap_active);
//                printf("Send -> %ld, i = %d\n", test_msgs[i], i);
                i++;
                multi_send--;
            }
            CAS(&test_queue.que_id, ENQUEUING, READY_TO_DEQUEUE);
            if(i >= TEST_NUM) break;
            
            if(i % 100000 == 0) {
                log_enqueue("enqueue msgs %ld\n", i);
            }
        }
    }
    log_enqueue("enqueue exit.\n");
    pthread_exit(NULL);
}

inline int test_msg_handler(test_msgs_t *pmsg){
    if(pmsg->magic != TEST_MSG_MAGIC) {
        return 0; //消息错误
    }

    /* 消息处理流程 */
    
    return 1; //消息成功
}

void *dequeue_task(void*arg){
    
    set_cpu_affinity(1);

    int i =0, j = 0;
    uint64_t total_msgs = 0;
    uint64_t error_msgs = 0;
    uint64_t latency_total = 0;
    test_msgs_t *pmsg;
    
    while(1) {
        if(CAS(&test_queue.que_id, READY_TO_DEQUEUE, DEQUEUING)) {

            if(test_queue.bitmap_maxbits < 0) {
                continue;
            }

            for(j=0;j<=test_queue.bitmap_maxbits;j++) {
                if(BITS_ISSET(j, &test_queue.bitmap_active)) {
                    
                    pmsg = (test_msgs_t*)test_queue.data[j];
                    if(pmsg->magic != TEST_MSG_MAGIC) {
                        error_msgs++;
                    }
                    error_msgs += test_msg_handler(pmsg)?0:1;
//                    printf("Recv -> %ld, i = %d\n", *pmsg, i);
                    BITS_CLR(j, &test_queue.bitmap_active);
                    i++;
                    total_msgs++;
                }
            }
            test_queue.bitmap_maxbits = -1;
            
            latency_total += RDTSC() - latency;
            latency=0;
            CAS(&test_queue.que_id, DEQUEUING, READY_TO_ENQUEUE);
//            printf("recv ok i = %d.\n", i);
            if(i >= TEST_NUM) break;

            if(i % 100000 == 0) {
                log_dequeue("dequeue msgs %ld, err %ld\n", i, error_msgs);
            }
        }
    }
    
    printf("dequeue. per ticks %lf, per msgs \033[1;31m%lf ns\033[m, msgs (recv %ld, err %ld, total %ld).\n", 
            latency_total*1.0/TEST_NUM/MULTI_SEND, 
            latency_total*1.0/TEST_NUM/MULTI_SEND/3000000000*1000000000,
            total_msgs, error_msgs, TEST_NUM);
    
    log_dequeue("dequeue exit.\n");

    pthread_exit(NULL);
}



int main()
{
    pthread_t enqueue_taskid, dequeue_taskid;

    int i =0;
    test_msgs_t *test_msgs = malloc(sizeof(test_msgs_t)*TEST_NUM);
    for(i=0;i<TEST_NUM;i++) {
        test_msgs[i].magic = TEST_MSG_MAGIC + (i%10000==0?1:0); //有错误的消息
        test_msgs[i].value = i+1;
    }

    pthread_create(&enqueue_taskid, NULL, enqueue_task, test_msgs);
    pthread_create(&dequeue_taskid, NULL, dequeue_task, NULL);

    pthread_join(enqueue_taskid, NULL);
    pthread_join(dequeue_taskid, NULL);

    free(test_msgs);
    
    return EXIT_SUCCESS;
}


