#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

#include "async_ring.h"

#include "common.h"

#define TID getpid()

#define log_enqueue(fmt...)  do{printf("\033[33m[%d]", TID);printf(fmt);printf("\033[m");}while(0)
#define log_dequeue(fmt...)  do{printf("\033[32m[%d]", TID);printf(fmt);printf("\033[m");}while(0)


#define STAT_INTERVAL_NLOOP 1000000


struct test_data_struct {
    int msg_type;
    int msg_code;
    int msg_len;
    unsigned long timestamp;
    
    struct timeval tv;
    
    char data[0];
#define MSG_TYPE    0xafafafaf
#define MSG_CODE    0xbfbfbfbf
#define MSG_LEN     512
};


struct async_ring *ring = NULL;



pthread_key_t keyenqueue_count;
pthread_key_t keydequeue_count;
pthread_key_t keydequeue_count_err;



void sig_handler(int signum)
{
    switch(signum) {
        case SIGINT:
        case SIGKILL:
            async_ring_dump(stderr, ring);
            async_ring_free(ring);
        default:;
    }
    printf("\033[m Catch Ctrl-C.\n");
    exit(1);
}

void *enqueue_ring(void *arg) 
{
    unsigned long int enqueue_count = 0;
    
    pthread_setspecific(keyenqueue_count, (void*)enqueue_count);
    
    struct test_data_struct *data = NULL;
    while(1) {
        if(async_ring_full(ring)) {
            continue;
        }
        data = malloc(MSG_LEN);
        data->msg_type = MSG_TYPE;
        data->msg_code = MSG_CODE;
        data->msg_len = MSG_LEN;

        /* 时间戳 */
        set_timestamp(&data->timestamp);
        gettimeofday(&data->tv, NULL);
        
        if(async_ring_enqueue(ring, data) != 0) {
            printf("Enqueue error.\n");
        } else {
            enqueue_count++;
            if(enqueue_count % STAT_INTERVAL_NLOOP == 0) {
                log_enqueue("enqueue %ld.\n", enqueue_count);
                
            }
        }
#if STAT_INTERVAL_NLOOP<=10
        sleep(1);
#endif
    }
    pthread_exit(NULL);
}

void *dequeue_ring(void *arg)
{
    
    unsigned long int dequeue_count = 0;
    unsigned long int dequeue_count_err = 0;
    struct test_data_struct *data = NULL;
    unsigned long ticks = 0, latencyticks_total = 0;
    struct timeval tv;
    unsigned long int diff_usec = 0, diff_usec_total = 0;
    
    pthread_setspecific(keydequeue_count, (void*)dequeue_count);
    pthread_setspecific(keydequeue_count_err, (void*)dequeue_count_err);
    
    while(1) {
        if(async_ring_empty(ring)) {
            continue;
        }
        if(async_ring_dequeue(ring, (void**)&data) != 0) {
            printf("Dequeue error.\n");
        } else {

            ticks = call_timestamp_diff(data->timestamp);
            latencyticks_total += ticks;

            gettimeofday(&tv, NULL);
            diff_usec = diff_timeval_usec(&tv, &data->tv);
            diff_usec_total += diff_usec;
            
            dequeue_count++;
            if(data->msg_type != MSG_TYPE || data->msg_code != MSG_CODE || data->msg_len != MSG_LEN) {
                dequeue_count_err++;
//                log_dequeue("dequeue %x, %x, %x.\n", data->msg_code, data->msg_type, data->msg_len);
            }
            if(dequeue_count % STAT_INTERVAL_NLOOP == 0) {
                log_dequeue("dequeue %ld, err = %ld. "\
                                "latency(Total %ld/%ld, %lf/%lf ms)"\
                                "(Total %ld/%ld, %lf us)\n", \
                                dequeue_count, dequeue_count_err, \
                                ticks, latencyticks_total,
                                ticks*1.0/CPU_MHZ[CPU_2_7GMHZ].freq*1000.0,
                                latencyticks_total*1.0/(dequeue_count-1)/CPU_MHZ[CPU_2_7GMHZ].freq*1000.0,
                                diff_usec, diff_usec_total, 
                                diff_usec_total*1.0/(dequeue_count-1));
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
    
    pthread_key_create(&keyenqueue_count, NULL);
    pthread_key_create(&keydequeue_count, NULL);
    pthread_key_create(&keydequeue_count_err, NULL);

    for(i=0;i<nr_enqueue_thread;i++) {
        pthread_create(&enqueue_threads[i], NULL, enqueue_ring, NULL);
    }
    for(i=0;i<nr_dequeue_thread;i++) {
        pthread_create(&dequeue_threads[i], NULL, dequeue_ring, NULL);
    }

//    sleep(5);
    
    for(i=0;i<nr_enqueue_thread;i++) {
        pthread_join(enqueue_threads[i], NULL);
    }
    for(i=0;i<nr_dequeue_thread;i++) {
        pthread_join(dequeue_threads[i], NULL);
    }
    printf("threads join.\n");
    
}

