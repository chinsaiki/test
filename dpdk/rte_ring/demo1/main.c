#include <assert.h>
#include <pthread.h>
#include <signal.h>

#include "rte_ring.h"

struct data_struct {
    int msg_type;
    int msg_code;
    int msg_len;
    char data[0];
};

int nr_of_queue_node = 4096;

struct rte_ring *ring = NULL;



void sig_handler(int signum)
{
    printf("Catch Ctrl+c\n");
    switch(signum) {
        case SIGINT:
        case SIGKILL:
            rte_ring_free(ring);
        default:;
    }
}

void *enqueue_ring(void *arg) 
{
    struct data_struct *data = NULL;
    while(1) {
        if(rte_ring_full(ring)) {
//            printf("Full.\n");
            continue;
        }
        data = malloc(512);
        assert(rte_ring_enqueue(ring, data) == 0);
    }
}

void *dequeue_ring(void *arg)
{
    struct data_struct *data = NULL;
    while(1) {
        if(rte_ring_empty(ring)) {
//            printf("Empty.\n");
            continue;
        }
        assert(rte_ring_dequeue(ring, (void**)&data) == 0);
        free(data);
    }
}

int main(int argc,char *argv[])
{
    int i;
    pthread_t penqueue1;
    pthread_t pdequeue1;
    ring = rte_ring_create("test",nr_of_queue_node, 0);

    pthread_create(&penqueue1, NULL, enqueue_ring, NULL);
    pthread_create(&pdequeue1, NULL, dequeue_ring, NULL);
    
    
    while(1) {
        sleep(1);
    }
    
    
}
