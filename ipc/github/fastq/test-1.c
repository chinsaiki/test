#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdint.h>
#include <pthread.h>

#include <fastq.h>


#ifndef TEST_NUM
#define TEST_NUM   (1UL<<20)
#endif

#define MODULE_1 1
#define MODULE_2 2


/**
 *  获取tick
 */
#ifndef RDTSC
#define RDTSC() ({\
    register uint32_t a,d; \
    __asm__ __volatile__( "rdtsc" : "=a"(a), "=d"(d)); \
    (((uint64_t)a)+(((uint64_t)d)<<32)); \
    })
#endif

typedef struct  {
#define TEST_MSG_MAGIC 0x123123ff    
    int magic;
    unsigned long value;
    uint64_t latency;
}__attribute__((aligned(64))) test_msgs_t;


test_msgs_t *test_msgs;
uint64_t latency_total = 0;
uint64_t total_msgs = 0;
uint64_t error_msgs = 0;
struct fastq_context ctx1;

void *enqueue_task(void*arg){
    struct fastq_context *ctx = (struct fastq_context *)arg;
    int i =0;
    test_msgs_t *pmsg = NULL;
    unsigned long send_cnt = 0;
    
    while(1) {
//        usleep(1000000);
        pmsg = &test_msgs[i++%TEST_NUM];
        pmsg->latency = RDTSC();
        unsigned long addr = (unsigned long)pmsg;
//        printf("send addr = %lx(%p)\n", pmsg, pmsg);
        fastq_send_main(ctx, MODULE_1, MODULE_2, &addr, sizeof(unsigned long));

        send_cnt++;

        if(send_cnt % 17109000 == 0) {
            sleep(3);
        }

    }
    pthread_exit(NULL);
}

void handler_test_msg(void* msg)
{
//    usleep(1000000);
    size_t sz = sizeof(unsigned long);
    test_msgs_t *pmsg;
    
    pmsg = (test_msgs_t *)msg;
//    printf("recv addr = %lx(%p) %ld\n", pmsg, pmsg, pmsg->value);

    latency_total += RDTSC() - pmsg->latency;
    pmsg->latency = 0;
    if(pmsg->magic != TEST_MSG_MAGIC) {
        error_msgs++;
    }
    
    total_msgs++;

    if(total_msgs % 1000 == 0) {
        printf("dequeue. per msgs \033[1;31m%lf ns\033[m, msgs (total %ld, err %ld).\n", 
                latency_total*1.0/total_msgs/3000000000*1000000000,
                total_msgs, error_msgs);
    }

}

void *dequeue_task(void*arg) {
    struct fastq_context *ctx = (struct fastq_context *)arg;

    fastq_recv_main(ctx, MODULE_1, MODULE_2, handler_test_msg);
    
    pthread_exit(NULL);
}



int sig_handler(int signum) {

    fastq_ctx_print(stderr, &ctx1);

    exit(1);
}

int main()
{
    pthread_t enqueue_taskid, dequeue_taskid;

    
    signal(SIGINT, sig_handler);
    
    fastq_create(&ctx1, 4, 8, sizeof(unsigned long));
    unsigned int i =0;
    test_msgs = (test_msgs_t *)malloc(sizeof(test_msgs_t)*TEST_NUM);
    for(i=0;i<TEST_NUM;i++) {
        test_msgs[i].magic = TEST_MSG_MAGIC + (i%10000==0?1:0); //有错误的消息
//        test_msgs[i].magic = TEST_MSG_MAGIC; //有错误的消息
        test_msgs[i].value = i+1;
    }


    pthread_create(&enqueue_taskid, NULL, enqueue_task, &ctx1);
    pthread_create(&dequeue_taskid, NULL, dequeue_task, &ctx1);

    pthread_join(enqueue_taskid, NULL);
    pthread_join(dequeue_taskid, NULL);

    return EXIT_SUCCESS;
}



