//下面是一段在Linux 平台下能引起线程死锁的小例子。
//
//这个实例程序仅仅是使用了条件变量和互斥量进行一个简单的线程同步，thread0 首先启动，
//锁住互斥量 mutex，然后调用 pthread_cond_wait，它将线程 tid[0] 放在等待条件的线程列表上后，
//对 mutex 解锁。thread1 启动后等待 10 秒钟，此时 pthread_cond_wait 应该已经将 mutex 解锁，
//这时 tid[1] 线程锁住 mutex，然后广播信号唤醒 cond 等待条件的所有等待线程，之后解锁 mutex。
//当 mutex 解锁后，tid[0] 线程的 pthread_cond_wait 函数重新锁住 mutex 并返回，最后 tid[0] 
//再对 mutex 进行解锁。
//
//https://www.cnblogs.com/mydomain/archive/2011/08/15/2139830.html



#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

#ifdef DEADLOCK
/**
 *  由于线程被取消，但并未释放锁，这里会产生死锁
 */
void* thread0(void* arg)
{
    pthread_mutex_lock(&mutex);
    printf("in thread 0 tag 1\n");
    pthread_cond_wait(&cond, &mutex);
    printf("in thread 0 tag 2\n");
    pthread_mutex_unlock(&mutex);
    printf("in thread 0 tag 3\n");
    pthread_exit(NULL);
}

#else
/**
 *  使用 pthread_cleanup_push 解决死锁问题
 */
void cleanup(void *arg)
{
    pthread_mutex_unlock(&mutex);
}
void* thread0(void* arg)
{
    pthread_cleanup_push(cleanup, NULL); // thread cleanup handler
    pthread_mutex_lock(&mutex);
    printf("in thread 0 tag 1\n");
    pthread_cond_wait(&cond, &mutex);
    printf("in thread 0 tag 2\n");
    pthread_mutex_unlock(&mutex);
    printf("in thread 0 tag 3\n");
    pthread_cleanup_pop(0);
    pthread_exit(NULL);
}
#endif

void* thread1(void* arg)
{
    sleep(10);
    printf("in thread 1 tag 1\n");
    pthread_mutex_lock(&mutex);
    printf("in thread 1 tag 2\n");
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
    printf("in thread 1 tag 3\n");
    pthread_exit(NULL);
}
int main()
{
    pthread_t tid[2];
    if (pthread_create(&tid[0], NULL, thread0, NULL) != 0) 
    {
        exit(1);
    }
    if (pthread_create(&tid[1], NULL, thread1, NULL) != 0) 
    {
        exit(1);
    }
    sleep(5);
    printf("in main thread tag 1\n");
    pthread_cancel(tid[0]);

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    return 0;
}

