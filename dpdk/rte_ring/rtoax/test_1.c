#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <malloc.h>
#include <stdint.h>

#include "ring.h"

#define log_enqueue(fmt...)  do{printf("\033[33m[%d]", getpid());printf(fmt);printf("\033[m");}while(0)
#define log_dequeue(fmt...)  do{printf("\033[32m[%d]", getpid());printf(fmt);printf("\033[m");}while(0)

void sig_handler(int signum)
{
    switch(signum) {
        case SIGINT:
        case SIGKILL:
        default:;
    }
    printf("\033[m Catch Ctrl-C.\n");
    exit(1);
}

static atomic64_t enqueue_count = {0};
static atomic64_t enqueue_count_err = {0};
static atomic64_t dequeue_count = {0};
static atomic64_t dequeue_count_err = {0};

static atomic64_t queue_tx_err = {0};


void *enqueue_ring(void *arg) 
{
    char *str = "rtoax testing";
    int i=0;
    while(1) {
//        usleep(10000);
        if(RING_IS_OK == ring_force_enqueue((struct ring_struct *)arg, str)) {
            atomic64_inc(&enqueue_count);
        } else {
            atomic64_inc(&enqueue_count_err);
        }
        printf("enqueue %ld, err %ld.\n", atomic64_read(&enqueue_count), atomic64_read(&enqueue_count_err));
        if(atomic64_read(&enqueue_count)+atomic64_read(&enqueue_count_err) == 1003) {
            break;
        }
    }
    pthread_exit(NULL);
}

void *dequeue_ring(void *arg)
{
    char *str;
    
    usleep(1000000);
    while(1) {
//        usleep(1000000);
        if(RING_IS_OK == ring_force_dequeue((struct ring_struct *)arg, &str)) {
            atomic64_inc(&dequeue_count);
        } else {
            atomic64_inc(&dequeue_count_err);
        }
        printf("dequeue %ld, err %ld.\n", atomic64_read(&dequeue_count), atomic64_read(&dequeue_count_err));
        if(atomic64_read(&dequeue_count)+atomic64_read(&dequeue_count_err) == 1000) {
            break;
        }
    }
    pthread_exit(NULL);
}

int main(int argc,char *argv[])
{
    int i;

    int nr_enqueue_thread = 1;
    int nr_dequeue_thread = 1;
    
    int ring_size = 16;

    if (argc == 4) {
        nr_enqueue_thread = atoi(argv[1]); 
        nr_dequeue_thread = atoi(argv[2]); 
        ring_size = atoi(argv[3]); 
    } else {
        printf("usage: %s [nthread-enqueue] [nthread-dequeue] [ring-size].\n", argv[0]);
        exit(1);
    } 
    struct ring_struct *ring = ring_create(NULL, ring_size);
    
    
    pthread_t enqueue_threads[nr_enqueue_thread];
    pthread_t dequeue_threads[nr_dequeue_thread];

    signal(SIGINT, sig_handler);
    

    for(i=0;i<nr_enqueue_thread;i++) {
        pthread_create(&enqueue_threads[i], NULL, enqueue_ring, ring);
    }
    for(i=0;i<nr_dequeue_thread;i++) {
        pthread_create(&dequeue_threads[i], NULL, dequeue_ring, ring);
    }
    
    for(i=0;i<nr_enqueue_thread;i++) {
        pthread_join(enqueue_threads[i], NULL);
    }
    for(i=0;i<nr_dequeue_thread;i++) {
        pthread_join(dequeue_threads[i], NULL);
    }
    ring_dump(stdout, ring);
    ring_destroy(ring);
    printf("threads join.\n");
    
}

