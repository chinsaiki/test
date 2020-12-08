#include <pthread.h>
#include <stdio.h>

void* test_task_fn_not_cancel_safe(void* unused)
{
	printf("test_task_fn.\n");

    /**
    * 线程准备执行一些关键工作，在这个过程中不希望被取消。
    * 所以先通过pthread_setcancelstate()将本线程的cancel state
    * 设为disabled。
    */
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

    printf("cancel state disabled.\n");
    sleep(1);
    
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
    printf("Nobody give me a cancel.\n");

    

    /**
    * 从这里开始，函数里不再包含cancellation point了。
    * 如果收到取消请求，将无法取消。所以先把本线程的cancel type
    * 设为asynchronous异步，收到取消请求将立即取消。
    */
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    /* 不包含cancellation point的代码 */

    
    sleep(2);
    printf("Try self exit %ld\n", pthread_self());

    static int status = 12121;
    
    pthread_exit(&status);
	return NULL;
}

void cleanup_handler (size_t size)
{
    printf("cleanup_handler.\n");
}


void* test_task_fn_is_cancel_safe(void* unused)
{
	printf("test_task_fn.\n");

	/* Register a cleanup handler for this buffer, to deallocate it in
	 * case the thread exits or is cancelled. */
	pthread_cleanup_push (cleanup_handler, NULL);;

    /**
    * 线程准备执行一些关键工作，在这个过程中不希望被取消。
    * 所以先通过pthread_setcancelstate()将本线程的cancel state
    * 设为disabled。
    */
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

    printf("cancel state disabled.\n");
    sleep(1);
    
    /**
    * 关键工作执行完成，可以被取消。
    * 通过pthread_setcancelstate()将本线程的cancel state
    * 设为enabled。
    */
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

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
    printf("Nobody give me a cancel.\n");

    

    /**
    * 从这里开始，函数里不再包含cancellation point了。
    * 如果收到取消请求，将无法取消。所以先把本线程的cancel type
    * 设为asynchronous异步，收到取消请求将立即取消。
    */
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    /* 不包含cancellation point的代码 */

    
    sleep(2);
//    printf("Try self exit %ld\n", pthread_self());

	/* Unregister the cleanup handler. Because we pass a nonzero value,
	 * this actually performs the cleanup by calling
	 * deallocate_buffer. */
	pthread_cleanup_pop (1);

    static int status = 12121;
    

    
    pthread_exit(&status);
	return NULL;
}


void demo1_cancel_segvfault() {

    int *pstatus;
	pthread_t thread_id;
    
	pthread_create(&thread_id, NULL, test_task_fn_not_cancel_safe, NULL);

    sleep(2);
    printf("Try cancel %ld\n", thread_id);

    /**********************************************************\
        这里在线程 pthread_exit 之前cancel，会触发段错误
    \**********************************************************/
    pthread_cancel(thread_id);

	pthread_join(thread_id, (void**)&pstatus);

    printf("pstatus = %d\n", *pstatus);
}



void demo1_cancel_safe() {

    int *pstatus;
	pthread_t thread_id;
    
	pthread_create(&thread_id, NULL, test_task_fn_is_cancel_safe, NULL);

    sleep(2);
    printf("Try cancel %ld\n", thread_id);

    /**********************************************************\
        这里在线程 pthread_exit 之前cancel，会触发段错误
    \**********************************************************/
    pthread_cancel(thread_id);

    /**********************************************************\
        这里在线程 pthread_exit 之前cancel，会触发段错误
    \**********************************************************/
	pthread_join(thread_id, (void**)&pstatus);

    printf("pstatus = %d\n", *pstatus);
}


/* The main program. */
int main ()
{
//    demo1_cancel_segvfault();
    demo1_cancel_safe();
    
}

