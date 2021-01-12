#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "async_ring.h"

#define TEST_NUMBER 2<<16

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
    struct async_ring *ring = (struct async_ring *)data;
    int i = 0;
    int ret;
    cc_queue_node_t *p;

    for (i=0; i < TEST_NUMBER; i++) {
        p = (cc_queue_node_t *)malloc(sizeof(cc_queue_node_t));
        p->data = i;
        p->magic = MAGIC;
enqueue:    
        ret = async_ring_mp_enqueue(ring, p);
        if (ret != 0) {
//            printf("enqueue failed: %d\n", i);
            goto enqueue;
        }
    }

    return NULL;
}

void *dequeue_func(void *data)
{
    struct async_ring *ring = (struct async_ring *)data;
    int ret;
    int i = 0;
    cc_queue_node_t *p;
    ticks t1, t2, diff;

    t1 = getticks();
    while (1) {
        p = NULL;
dequeue:    
        ret = async_ring_mc_dequeue(ring, (void **)&p);
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
    
    struct async_ring *ring;
    
    ring = async_ring_create("test",4, ASYNC_RING_F_SP_ENQ|ASYNC_RING_F_SC_DEQ);
    
    pthread_create(&pid1, NULL, enqueue_fun, ring);

    pthread_create(&pid6, NULL, dequeue_func, ring);

    pthread_join(pid1, NULL);

    pthread_join(pid6, NULL);

    async_ring_dump(stderr, ring);

    async_ring_free(ring);


    return 0;
}

