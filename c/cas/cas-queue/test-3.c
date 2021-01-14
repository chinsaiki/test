/**
 *  test-3.c
 *  
 *  夹带多消息（内存指针）的发送接收，使用封装的API接口
 *  
 *  荣涛  2021年1月14日
 *  
 */
#define _GNU_SOURCE
#include <sched.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

#include "cas-queue.h"
#include "log.h"


#ifndef TEST_NUM
#define TEST_NUM   (1UL<<20)
#endif
#ifndef MULTI_SEND
#define MULTI_SEND  10
#endif

typedef struct test_msgs_s {
#define TEST_MSG_MAGIC 0x123123ff    
    int magic;
    unsigned long value;
    uint64_t latency;
}__attribute__((aligned(64))) test_msgs_t;


fast_queue_t * my_queue;

void *enqueue_task(void*arg){
    
    int i =0, nmsg = 0, ret;
    test_msgs_t *pmsgs = (test_msgs_t *)arg;


    while(1) {
        if(!fast_queue_can_add(my_queue)) continue;
    
        for(i=0; i<MULTI_SEND && nmsg < TEST_NUM; i++, nmsg++) {
            pmsgs[nmsg].latency = RDTSC();
            fast_queue_add_msgs(my_queue, i, &pmsgs[nmsg]);
        }
        fast_queue_add_msgs_done(my_queue);
        fast_queue_send(my_queue);
        
        if(nmsg >= TEST_NUM) break;
    }
    log_enqueue("enqueue exit.\n");
    pthread_exit(NULL);
}


uint64_t latency_total = 0;
uint64_t total_msgs = 0;
uint64_t error_msgs = 0;


inline int msg_handler(void *msg){
    test_msgs_t *pmsg = (test_msgs_t *)msg;

//    printf("handle %d\n", pmsg->value);
    
    latency_total += RDTSC() - pmsg->latency;
    total_msgs++;
        
    if(pmsg->magic != TEST_MSG_MAGIC) {
        error_msgs++;
        return 0; //消息错误
    }

    /* 消息处理流程 */
    
    return 1; //消息成功
}

void *dequeue_task(void*arg){
    int nr_msg = 0;
        
    while(1) {
        nr_msg += fast_queue_recv(my_queue, msg_handler);

        
        if(nr_msg >= TEST_NUM) break;
    }
    
    log_dequeue("dequeue exit.\n");
    
    printf("dequeue. per ticks %lf, per msgs \033[1;31m%lf ns\033[m, msgs (recv %ld, err %ld, total %ld).\n", 
            latency_total*1.0/total_msgs, 
            latency_total*1.0/total_msgs/3000000000*1000000000,
            total_msgs, error_msgs, total_msgs);
    
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
    
    my_queue = fast_queue_create();

    pthread_create(&enqueue_taskid, NULL, enqueue_task, test_msgs);
    pthread_create(&dequeue_taskid, NULL, dequeue_task, NULL);

    pthread_join(enqueue_taskid, NULL);
    pthread_join(dequeue_taskid, NULL);

    free(test_msgs);
    fast_queue_destroy(my_queue);
    
    return EXIT_SUCCESS;
}



