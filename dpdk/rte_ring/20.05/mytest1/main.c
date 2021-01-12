#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "rte_ring.h"

#define TEST_NUMBER 2<<16
#if 1

typedef struct cc_queue_node {
    int magic;
#define MAGIC 0x12321567    
    int data;
} cc_queue_node_t;


typedef unsigned long long ticks;

static inline ticks getticks(void)
{
    uint32_t a, d;

    asm volatile("rdtsc" : "=a" (a), "=d" (d));
    return (((ticks)a) | (((ticks)d) << 32));
}


void *enqueue_fun(void *data)
{
    struct rte_ring *ring = (struct rte_ring *)data;
    int i = 0;
    int ret;
    cc_queue_node_t *p;

    for (i=0; i < TEST_NUMBER; i++) {
        p = (cc_queue_node_t *)malloc(sizeof(cc_queue_node_t));
        p->data = i;
        p->magic = MAGIC;
enqueue:    
        ret = rte_ring_mp_enqueue(ring, p);
        if (ret != 0) {
//            printf("enqueue failed: %d\n", i);
            goto enqueue;
        }
    }

    return NULL;
}

void *dequeue_func(void *data)
{
    struct rte_ring *ring = (struct rte_ring *)data;
    int ret;
    int i = 0;
    cc_queue_node_t *p;
    ticks t1, t2, diff;

    t1 = getticks();
    while (1) {
        p = NULL;
dequeue:    
        ret = rte_ring_mc_dequeue(ring, (void **)&p);
        if (ret != 0) {
            goto dequeue;
            continue;
        }
        if (p != NULL) {
//            printf("dequeue_func get p.\n");
            i++;
            if(p->magic != MAGIC) {
                printf("WRONG MAGIC, WRONG MAGIC, WRONG MAGIC, WRONG MAGIC\n");
            }
            free(p);
            if (i == TEST_NUMBER) {
                printf("Done.\n");
                break;
            }
            p = NULL;
        }
    }

    t2 = getticks();
    diff = t2 - t1;
    printf("time diff: %llu\n", diff);

    return NULL;
}


int main(int argc, char *argv[])
{
    int ret = 0;
    pthread_t pid1, pid2, pid6, pid7;
    
    struct rte_ring *ring;

//    ring = malloc(sizeof(struct rte_ring));
//    if (ring == NULL) {
//        return -1;
//    }
//    rte_ring_init(ring, "test", 4, 0);
    
//    printf("ring addr = %p\n", ring);
//
    ring = rte_ring_create("test",4, 0, 0);
//
//
//    printf("start enqueue\n");
//
    pthread_create(&pid1, NULL, enqueue_fun, ring);
//
//    pthread_create(&pid2, NULL, enqueue_fun, ring);
//
//    printf("start dequeue, consumer thread.\n");
//
    pthread_create(&pid6, NULL, dequeue_func, ring);
//    pthread_create(&pid7, NULL, dequeue_func, ring);
//    
    pthread_join(pid1, NULL);
//    pthread_join(pid2, NULL);
    pthread_join(pid6, NULL);
//    pthread_join(pid7, NULL);
//    
//    rte_ring_dump(stderr, ring);

//    printf("ring addr = %p\n", ring);
    rte_ring_free(ring);
//    free(ring);

//    ring = malloc(sizeof(struct rte_ring));
//    if (ring == NULL) {
//        return -1;
//    }
//    rte_ring_init(ring, "test", 4, 0);
//    free(ring);

    return 0;
}
#else
#include <assert.h>
#include <pthread.h>
#include <signal.h>

//#include "async_ring.h"

struct data_struct {
    int msg_type;
    int msg_code;
    int msg_len;
    char data[0];
};


struct rte_ring *ring = NULL;

volatile unsigned long int dequeue_count = 0;
volatile unsigned long int enqueue_count = 0;


void sig_handler(int signum)
{
    printf("Catch Ctrl+c\n");
    switch(signum) {
        case SIGINT:
        case SIGKILL:
            rte_ring_free(ring);
        default:;
    }
    exit(1);
}

void *enqueue_ring(void *arg) 
{
    struct data_struct *data = NULL;
    while(1) {
        if(rte_ring_full(ring)) {
            continue;
        }
        data = malloc(512);
        if(rte_ring_enqueue(ring, data) != 0) {
//            printf("Enqueue error.\n");
        } else {

            enqueue_count++;
            if(enqueue_count % 1000000 == 0) {
                printf("enqueue %ld, dequeue %ld.\n", enqueue_count, dequeue_count);
            }
        }
    }
}

void *dequeue_ring(void *arg)
{
    
    struct data_struct *data = NULL;
    while(1) {
        if(rte_ring_empty(ring)) {
            continue;
        }
        if(rte_ring_dequeue(ring, (void**)&data) != 0) {
//            printf("Dequeue error.\n");
        } else {

            dequeue_count++;
            if(dequeue_count % 1000000 == 0) {
                printf("dequeue %ld, enqueue %ld.\n", dequeue_count, enqueue_count);
            }
        }
        free(data);
    }
}

int main(int argc,char *argv[])
{
    int i;
    pthread_t penqueue1,penqueue2,penqueue3;
    pthread_t pdequeue1,pdequeue2,pdequeue3;

    signal(SIGINT, sig_handler);
    
    ring = rte_ring_create("test",1024, 0, 0);

    pthread_create(&penqueue1, NULL, enqueue_ring, NULL);
//    pthread_create(&penqueue2, NULL, enqueue_ring, NULL);
//    pthread_create(&penqueue3, NULL, enqueue_ring, NULL);

    pthread_create(&pdequeue1, NULL, dequeue_ring, NULL);

    
    
    while(1) {
        sleep(1);
    }
    
    
}

#endif
