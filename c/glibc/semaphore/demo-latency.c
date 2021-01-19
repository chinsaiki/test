/*************************************************************************
	> File Name: demo-latency.c
	> Author: 
	> Mail: 
	> Created Time: 2017年09月12日 星期二 23时40分59秒
	> Modify Time: 2021年1月19日 时延测量 荣涛
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdint.h>
#include<pthread.h>
#include<string.h>
#include<semaphore.h>
#include<time.h>

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

sem_t sem_empty;
sem_t sem_empty1;

unsigned long latency = 0;
unsigned long total_latency = 0;
unsigned long total_num = 0;

//消费者
void *consums(void *arg)
{
    int i = 0;
//    sem_wait(&sem_empty); 
//    sleep(1);
//    sem_post(&sem_empty);
    while(1)
    {
        sem_wait(&sem_empty1); 
        
        total_latency += RDTSC() - latency;
        total_num ++;

        if(total_num % 100000 == 0) {
            printf("latency. ticks %lf, per msgs \033[1;31m%lf ns\033[m, msgs (total %ld).\n", 
                total_latency*1.0/total_num, 
                total_latency*1.0/total_num/3000000000*1000000000, total_num);
        }
        sem_post(&sem_empty); 
    }

    return NULL;
}

//生产者
void *product(void *arg)
{
    int i = 0;
    sem_wait(&sem_empty1); 
    sleep(1);
    while(1)
    {
        sem_wait(&sem_empty); 
        latency = RDTSC();
        
        sem_post(&sem_empty1);
    }
    return NULL;
}

int main()
{
    int ret;
    pthread_t ptid,ctid;

    //初始化信号量
    sem_init(&sem_empty,0,1);
    sem_init(&sem_empty1,0,1);

    //创建生产者和消费者线程
    ret = pthread_create(&ptid,NULL,product,NULL);
    if(ret != 0)
    {
        fprintf(stderr,"pthread_create error: %s\n",strerror(ret));
        exit(1);
    }

    ret = pthread_create(&ctid,NULL,consums,NULL);
    if(ret != 0)
    {
        fprintf(stderr,"pthread_create error: %s\n",strerror(ret));
        exit(1);
    }

    //回收子线程
    pthread_join(ptid,NULL);
    pthread_join(ctid,NULL);

    //销毁信号量
    sem_destroy(&sem_empty);
    sem_destroy(&sem_empty1);

    return 0;
}

