#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "lockfree_queue.h"
#include "malloc_node_allocator.h"

#include "test_common.h"

#ifndef MAX_NUMBER
#define MAX_NUMBER 4
#endif

struct msgs_struct {
    int id;
    unsigned long timestamp;
};

lockfree_queue_t queue;

/* We're using malloc to allocate memory for queue nodes. */
lockfree_qnode_t *(*qnode_allocator)(void) = &malloc_node_allocator;
void(*qnode_deallocator)(lockfree_qnode_t *) = &malloc_node_deallocator;


void *enqueue_main(void *arg)
{
    int i;
    static struct msgs_struct msgs[MAX_NUMBER];
    memset(msgs, 0, sizeof(msgs));
    for(i=0;i<MAX_NUMBER;i++) {
        msgs[i].id = i+1;
    }
    /* Add numbers 1 to MAX to the queue. */
    for (i = 0; i < MAX_NUMBER; i++) {
        set_msg_timestamp(&msgs[i].timestamp);
        lockfree_queue_enqueue(&queue, (void *) &msgs[i]);
    }
    
    pthread_exit(NULL);
    return NULL;
}

void *dequeue_main(void *arg)
{
    /* Make sure all the numbers appear num_threads times. */
    struct msgs_struct *msg;
    unsigned long ticks_total = 0;
    
    do {
dequeue:        
        msg = (struct msgs_struct *) lockfree_queue_dequeue(&queue);
        if(!msg) goto dequeue;
        if (msg->id > MAX_NUMBER || msg->id < 0) {
            /* We got a bad value! */
            return NULL;
        }
        ticks_total += call_timestamp_diff(&msg->timestamp);
        
//        printf("id = %d\n", msg->id);
    }while (msg && msg->id != MAX_NUMBER && msg->id != 0);
    printf("Latency Per Message = %lf ms\n", ticks_total*1.0/(MAX_NUMBER-1)/CPU_MHZ[CPU_2_7GMHZ].freq*1000.0);

    pthread_exit(NULL);

    return NULL;
}


int main() {
    
    pthread_t task1, task2;

    lockfree_queues_initall();

    lockfree_queue_init(&queue);

    pthread_create(&task1, NULL, enqueue_main, NULL);
    pthread_create(&task2, NULL, dequeue_main, NULL);

    
    pthread_join(task1, NULL);
    pthread_join(task2, NULL);


    lockfree_reapd_cleanup();

    return 1;
}


