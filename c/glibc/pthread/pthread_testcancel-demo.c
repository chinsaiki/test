//根据POSIX标准，
//    pthread_join()、pthread_testcancel()、pthread_cond_wait()、
//    pthread_cond_timedwait()、sem_wait()、sigwait()等函数以及read()、write()等
//    会引起阻塞的系统调用都是Cancelation-point，而其他pthread函数都不会引起Cancelation动作。
//    但是pthread_cancel的手册页声称，由于LinuxThread库与C库结合得不好，
//    因而目前C库函数都不是Cancelation-point；
//    但CANCEL信号会使线程从阻塞的系统调用中退出，并置EINTR错误码，
//    因此可以在需要作为Cancelation-point的系统调用前后调用pthread_testcancel()，
//    从而达到POSIX标准所要求的目标，
//
//pthread_testcancel在不包含取消点，但是又需要取消点的地方创建一个取消点，
//    以便在一个没有包含取消点的执行代码线程中响应取消请求。
//
//https://www.cnblogs.com/mydomain/archive/2011/08/15/2139850.html


#include <pthread.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define handle_error_en(en, msg) \
       do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

static int done = 0;
static int cleanup_pop_arg = 0;
static int cnt = 0;

static void
cleanup_handler(void *arg)
{
   printf("Called clean-up handler\n");
   cnt = 0;
}

static void *
thread_start(void *arg)
{
   time_t start, curr;
   printf("New thread started\n");
   pthread_cleanup_push(cleanup_handler, NULL);
   curr = start = time(NULL);

   while (!done) {
       pthread_testcancel();           /* A cancellation point */
       if (curr < time(NULL)) 
       {
           curr = time(NULL);
           printf("cnt = %d\n", cnt);  /* A cancellation point */
           cnt++;
       }
   }

   pthread_cleanup_pop(cleanup_pop_arg);
   return NULL;
}

int
main(int argc, char *argv[])
{
   pthread_t thr;
   int s;
   void *res;

   s = pthread_create(&thr, NULL, thread_start, NULL);
   if (s != 0)
       handle_error_en(s, "pthread_create");

   sleep(2);           /* Allow new thread to run a while */

   if (argc > 1) 
   {
       if (argc > 2)
           cleanup_pop_arg = atoi(argv[2]);
       done = 1;

   } 
   else 
   {
       printf("Canceling thread\n");
       s = pthread_cancel(thr);
       if (s != 0)
           handle_error_en(s, "pthread_cancel");
   }

   s = pthread_join(thr, &res);
   if (s != 0)
       handle_error_en(s, "pthread_join");

   if (res == PTHREAD_CANCELED)
       printf("Thread was canceled; cnt = %d\n", cnt);
   else
       printf("Thread terminated normally; cnt = %d\n", cnt);
   exit(EXIT_SUCCESS);
}

