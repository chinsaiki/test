#include <assert.h>
#include <pthread.h>
#include <signal.h>

#include "async_ring.h"

struct data_struct {
    int msg_type;
    int msg_code;
    int msg_len;
    char data[0];
};


struct async_ring *ring = NULL;

volatile unsigned long int dequeue_count = 0;
volatile unsigned long int enqueue_count = 0;


void sig_handler(int signum)
{
    printf("Catch Ctrl+c\n");
    switch(signum) {
        case SIGINT:
        case SIGKILL:
            async_ring_free(ring);
        default:;
    }
    exit(1);
}

void *enqueue_ring(void *arg) 
{
    struct data_struct *data = NULL;
    while(1) {
        if(async_ring_full(ring)) {
            continue;
        }
        data = malloc(512);
        if(async_ring_enqueue(ring, data) != 0) {
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
        if(async_ring_empty(ring)) {
            continue;
        }
        if(async_ring_dequeue(ring, (void**)&data) != 0) {
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
    
    ring = async_ring_create("test",1024, 0);

    pthread_create(&penqueue1, NULL, enqueue_ring, NULL);
    pthread_create(&penqueue2, NULL, enqueue_ring, NULL);
    pthread_create(&penqueue3, NULL, enqueue_ring, NULL);

    pthread_create(&pdequeue1, NULL, dequeue_ring, NULL);

    
    
    while(1) {
        sleep(1);
    }
    
    
}
