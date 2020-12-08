#include <pthread.h>

enum
{
  PTHREAD_CANCEL_DEFERRED,
#define PTHREAD_CANCEL_DEFERRED	PTHREAD_CANCEL_DEFERRED
  PTHREAD_CANCEL_ASYNCHRONOUS
#define PTHREAD_CANCEL_ASYNCHRONOUS	PTHREAD_CANCEL_ASYNCHRONOUS
};


//pthread_setcancelstate, pthread_setcanceltype - set cancelability state and type
//int pthread_setcancelstate(int state, int *oldstate);
int pthread_setcanceltype(int type, int *oldtype);

//Compile and link with -pthread.
//（1）一个线程可以调用pthread_cancel来取消另一个线程。 
//（2）被取消的线程需要被join来释放资源。 
//（3）被取消的线程的返回值为PTHREAD_CANCELED 
//
//有关线程的取消，一个线程可以为如下三个状态： 
//  （1）可异步取消：一个线程可以在任何时刻被取消。 
//  （2）可同步取消：取消的请求被放在队列中，直到线程到达某个点，才被取消。
//  （3）不可取消：取消的请求被忽略。 
//    默认状态下，线程是可同步取消的。 
//
//调用pthread_setcanceltype来设定线程取消的方式： 
//        pthread_setcanceltype (PTHREAD_CANCEL_ASYNCHRONOUS, NULL); //异步取消、 
//        pthread_setcanceltype (PTHREAD_CANCEL_DEFERRED, NULL); //同步取消、 
//        pthread_setcanceltype (PTHREAD_CANCEL_DISABLE, NULL);//不能取消 

//一般来说，Posix的线程终止有两种情况：正常终止和非正常终止。
//线程主动调用pthread_exit()或者从线程函数中return都将使线程正常退出，
//这是可预见的退出方式；非正常终止是线程在其他线程的干预下，
//或者由于自身运行出错（比如访问非法地址）而退出，这种退出方式是不可预见的。 
// 最经常出现的情形是资源独占锁的使用：线程为了访问临界资源而为其加上锁，
// 但在访问过程中被外界取消，如果线程处于响应取消状态，且采用异步方式响应，
// 或者在打开独占锁以前的运行路径上存在取消点，则该临界资源将永远处于锁定状态得不到释放。
// 外界取消操作是不可预见的，因此的确需要一个机制来简化用于资源释放的编程。 
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

