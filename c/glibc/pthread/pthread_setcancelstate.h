#include <pthread.h>

/* Cancellation */
enum
{
  PTHREAD_CANCEL_ENABLE,
#define PTHREAD_CANCEL_ENABLE   PTHREAD_CANCEL_ENABLE
  PTHREAD_CANCEL_DISABLE
#define PTHREAD_CANCEL_DISABLE  PTHREAD_CANCEL_DISABLE
};


//pthread_setcancelstate, pthread_setcanceltype - set cancelability state and type
int pthread_setcancelstate(int state, int *oldstate);
//int pthread_setcanceltype(int type, int *oldtype);

//Compile and link with -pthread.
void  thread_function( void  *arg)
{
    /**
    * 线程准备执行一些关键工作，在这个过程中不希望被取消。
    * 所以先通过pthread_setcancelstate()将本线程的cancel state
    * 设为disabled。
    */
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    /* 执行关键工作 */
    ...
    /**
    * 关键工作执行完成，可以被取消。
    * 通过pthread_setcancelstate()将本线程的cancel state
    * 设为enabled。
    */
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    /**
    * 调用pthread_testcancel()函数，检查一下在cancel state
    * 为disabled状态的时候，是否有取消请求发送给本线程。
    * 如果有的话就取消（退出）。
    */
    pthread_testcancel();
    /**
    * pthread_testcancel()返回了，表明之前没有取消请求发送给本线程，
    * 继续其余的工作。
    * 这时候如果有取消请求发送给本线程，会在下一次执行到
    * cancellation point的时候（例如sleep(), read(), write(), ...）时取消。
    */
    ...
    /**
    * 从这里开始，函数里不再包含cancellation point了。
    * 如果收到取消请求，将无法取消。所以先把本线程的cancel type
    * 设为asynchronous，收到取消请求将立即取消。
    */
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    /* 不包含cancellation point的代码 */
    ...
}

