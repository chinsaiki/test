#include <assert.h>
#include <pthread.h>
#include <signal.h>

#include "async_ring.h"

#define log_enqueue(fmt...)  do{printf("\033[33m[%d]", getpid());printf(fmt);printf("\033[m");}while(0)
#define log_dequeue(fmt...)  do{printf("\033[32m[%d]", getpid());printf(fmt);printf("\033[m");}while(0)



struct test_data_struct {
    int msg_type;
    int msg_code;
    int msg_len;
    char data[0];
#define MSG_TYPE    0xafafafaf
#define MSG_CODE    0xbfbfbfbf
#define MSG_LEN     512
};


struct async_ring *ring = NULL;

volatile unsigned long int dequeue_count = 0;
volatile unsigned long int dequeue_count_err = 0;

volatile unsigned long int enqueue_count = 0;


void sig_handler(int signum)
{
    switch(signum) {
        case SIGINT:
        case SIGKILL:
            async_ring_free(ring);
        default:;
    }
    printf("\033[m Catch Ctrl-C.\n");
    exit(1);
}

void *enqueue_ring(void *arg) 
{
    struct test_data_struct *data = NULL;
    while(1) {
        if(async_ring_full(ring)) {
            continue;
        }
        data = malloc(MSG_LEN);
        data->msg_type = MSG_TYPE;
        data->msg_code = MSG_CODE;
        data->msg_len = MSG_LEN;
        
        if(async_ring_enqueue(ring, data) != 0) {
//            printf("Enqueue error.\n");
        } else {
            enqueue_count++;
            if(enqueue_count % 1000000 == 0) {
                log_enqueue("enqueue %ld, dequeue %ld.\n", enqueue_count, dequeue_count);
            }
        }
    }
    pthread_exit(NULL);
}

void *dequeue_ring(void *arg)
{
    
    struct test_data_struct *data = NULL;
    while(1) {
        if(async_ring_empty(ring)) {
            continue;
        }
        if(async_ring_dequeue(ring, (void**)&data) != 0) {
//            printf("Dequeue error.\n");
        } else {

            dequeue_count++;
            if(data->msg_type != MSG_TYPE || data->msg_code != MSG_CODE || data->msg_len != MSG_LEN) {
                dequeue_count_err++;
//                log_dequeue("dequeue %x, %x, %x.\n", data->msg_code, data->msg_type, data->msg_len);
            }
            if(dequeue_count % 1000000 == 0) {
                log_dequeue("dequeue %ld, enqueue %ld, err = %ld.\n", dequeue_count, enqueue_count, dequeue_count_err);
            }
            free(data);
        }
    }
    pthread_exit(NULL);
}

int main(int argc,char *argv[])
{
    int i;

    int nr_enqueue_thread = 10;
    int nr_dequeue_thread = 10;
    
    int ring_size = 1024;

    if (argc == 4) {
        nr_enqueue_thread = atoi(argv[1]); 
        nr_dequeue_thread = atoi(argv[2]); 
        ring_size = atoi(argv[3]); 
    } else {
        printf("usage: %s [nthread-enqueue] [nthread-dequeue] [ring-size].\n", argv[0]);
        exit(1);
    } 

    
    pthread_t enqueue_threads[nr_enqueue_thread];
    pthread_t dequeue_threads[nr_dequeue_thread];

    signal(SIGINT, sig_handler);
    
    ring = async_ring_create("test",ring_size, ASYNC_RING_F_EXACT_SZ);

    for(i=0;i<nr_enqueue_thread;i++) {
        pthread_create(&enqueue_threads[i], NULL, enqueue_ring, NULL);
    }
    for(i=0;i<nr_dequeue_thread;i++) {
        pthread_create(&dequeue_threads[i], NULL, dequeue_ring, NULL);
    }

    sleep(5);
    
    for(i=0;i<nr_enqueue_thread;i++) {
        pthread_join(enqueue_threads[i], NULL);
    }
    for(i=0;i<nr_dequeue_thread;i++) {
        pthread_join(dequeue_threads[i], NULL);
    }
    printf("threads join.\n");
    
}
