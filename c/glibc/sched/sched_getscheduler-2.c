#include <stdio.h>
#include <unistd.h>
#include <sched.h>

//ps -ef | grep watch
//root        102      2  0 2月08 ?       00:00:00 [watchdogd]
//./a.out 102
//Scheduler Policy for PID: 102  -> SCHED_FIFO


int main(int argc, char *argv[])
{
  pid_t pid;
  int policy;

  if (argc < 2)
    pid = 0;
  else
    pid = atoi(argv[1]);

  printf("Scheduler Policy for PID: %d  -> ", pid);

  policy = sched_getscheduler(pid);

  switch(policy) {
    case SCHED_OTHER: printf("SCHED_OTHER\n"); break;
    case SCHED_RR:   printf("SCHED_RR\n"); break;
    case SCHED_FIFO:  printf("SCHED_FIFO\n"); break;
    default:   printf("Unknown...\n");
  }
 }

