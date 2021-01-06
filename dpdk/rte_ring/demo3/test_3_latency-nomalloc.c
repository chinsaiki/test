#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

#include "async_ring.h"

#include "common.h"

/*
    测量结果好像和每次分配内存差不多，
[root@localhost demo3]# ./test_3_latency-nomalloc.out 3 1 2 
[-93989120]dequeue 1000000, err = 0. latency(Total 2484/2439308318, 0.000920/0.000903 ms)(Total 2/1666033, 1.666035 us)
[-93989120]dequeue 2000000, err = 0. latency(Total 1956/4836656976, 0.000724/0.000896 ms)(Total 1/3306649, 1.653325 us)
[-68811008]enqueue 1000000.
[-93989120]dequeue 3000000, err = 0. latency(Total 2076/7298557638, 0.000769/0.000901 ms)(Total 1/4995323, 1.665108 us)
[-85596416]enqueue 1000000.
[-77203712]enqueue 1000000.
*/

#define TID pthread_self()

#define log_enqueue(fmt...)  do{printf("\033[33m[%d]", TID);printf(fmt);printf("\033[m");}while(0)
#define log_dequeue(fmt...)  do{printf("\033[32m[%d]", TID);printf(fmt);printf("\033[m");}while(0)


#define STAT_INTERVAL_NLOOP 1000000
#define NR_MALLOC   (4096*1)

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
    unsigned long int enqueue_count = 0, i;
    
    pthread_setspecific(keyenqueue_count, (void*)enqueue_count);
    
    struct test_data_struct *data = memalign(4096, NR_MALLOC*MSG_LEN);
    assert(data);
    for(i=0;i<NR_MALLOC;i++) {
        data[i].msg_type = MSG_TYPE;
        data[i].msg_code = MSG_CODE;
        data[i].msg_len = MSG_LEN;
    }

    
    while(1) {
        if(async_ring_full(ring)) {
            continue;
        }

        /* 时间戳 */
        set_timestamp(&data[enqueue_count%NR_MALLOC].timestamp);
        gettimeofday(&data[enqueue_count%NR_MALLOC].tv, NULL);
enqueue:
        if(async_ring_enqueue(ring, &data[enqueue_count%NR_MALLOC]) != 0) {
//            printf("Enqueue error.\n");
            set_timestamp(&data[enqueue_count%NR_MALLOC].timestamp);
            goto enqueue;
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
dequeue:
        if(async_ring_dequeue(ring, (void**)&data) != 0) {
//            printf("Dequeue error.\n");
            goto dequeue;
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
    
    int flags = \
//        ASYNC_RING_F_SP_ENQ;
//        ASYNC_RING_F_SC_DEQ;
        ASYNC_RING_F_EXACT_SZ;
    
    ring = async_ring_create("test",ring_size, flags);
    
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


