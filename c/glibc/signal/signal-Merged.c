#include <signal.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

//https://www.gnu.org/software/libc/manual/html_node/Merged-Signals.html


//信号紧密合为一体
//如果在信号处理程序完全没有机会被调用之前将多个相同类型的信号传递到您的进程，
//则该处理程序只能被调用一次，就好像只有一个信号到达一样。 实际上，信号合并为一个。 
//当信号被阻塞时，或者在信号传递时系统正忙于运行其他进程的多处理环境中，可能会出现这种情况。 
//例如，这意味着您不能可靠地使用信号处理程序来计数信号。 您可以可靠地做出的唯一区分是，
//从过去的给定时间开始，是否至少有一个信号已经到达。
//
//这是 SIGCHLD 处理程序的示例，该处理程序补偿以下事实：接收到的信号数可能不等于生成它们的子进程数。 
//假定程序使用一系列结构跟踪所有子进程，如下所示：


struct process
{
    struct process *next;
    /* The process ID of this child.  */
    int pid;
    /* The descriptor of the pipe or pseudo terminal
     on which output comes from this child.  */
    int input_descriptor;
    /* Nonzero if this process has stopped or terminated.  */
    sig_atomic_t have_status;
    /* The status of this child; 0 if running,
     otherwise a status value from waitpid.  */
    int status;
};

struct process *process_list;

//此示例还使用一个标志来指示信号是否从过去的某个时间到达（程序最后一次将其清零时）。
/* Nonzero means some child’s status has changed
   so look at process_list for the details.  */
int process_status_change;

void sigchld_handler (int signo)
{
  int old_errno = errno;

  while (1) {
    register int pid;
    int w;
    struct process *p;

    /* Keep asking for a status until we get a definitive result.  */
    do
      {
        errno = 0;
        pid = waitpid (WAIT_ANY, &w, WNOHANG | WUNTRACED);
      }
    while (pid <= 0 && errno == EINTR);

    if (pid <= 0) {
      /* A real failure means there are no more
         stopped or terminated child processes, so return.  */
      errno = old_errno;
      return;
    }

    /* Find the process that signaled us, and record its status.  */

    for (p = process_list; p; p = p->next)
      if (p->pid == pid) {
        p->status = w;
        /* Indicate that the status field
           has data to look at.  We do this only after storing it.  */
        p->have_status = 1;

        /* If process has terminated, stop waiting for its output.  */
        if (WIFSIGNALED (w) || WIFEXITED (w))
          if (p->input_descriptor)
            FD_CLR (p->input_descriptor, &input_wait_mask);

        /* The program should check this flag from time to time
           to see if there is any news in process_list.  */
        ++process_status_change;
      }

    /* Loop around to handle all the processes
       that have something to tell us.  */
  }
}

if (process_status_change) {
  struct process *p;
  process_status_change = 0;
  for (p = process_list; p; p = p->next)
    if (p->have_status) {
//      … Examine p->status …
    }
}

//在检查列表之前清除标志非常重要；否则，如果信号刚好在清除标志之前以及在检查了过程列
//表的相应元素之后才发出，则状态更改将不会被注意，直到下一个信号到达以再次设置标志为止。
//您当然可以通过在扫描列表时阻止信号来避免此问题，但是通过按正确的顺序进行操作来保证正确性要优雅得多。
//
//检查进程状态的循环避免检查p-> status，直到看到状态已被有效存储。这是为了确保状态在访问
//过程中不会更改。一旦设置了p-> have_status，则意味着子进程已停止或终止，并且在任何一种情况下，
//它都无法再次停止或终止，直到程序注意到此为止。有关在变量访问期间应对中断的更多信息，请参见“原子用法”。
//
//这是您可以测试处理程序自上次检查以来是否已运行的另一种方法。该技术使用的计数器永远不会在
//处理程序外部更改。该程序不会清除计数，而是会记住先前的值，并查看自从先前的检查以来它是否已更改。
//这种方法的优点是程序的不同部分可以独立检查，每个部分检查自从上次检查该部分以来是否有信号。

sig_atomic_t process_status_change;

sig_atomic_t last_process_status_change;

//…
{
  sig_atomic_t prev = last_process_status_change;
  last_process_status_change = process_status_change;
  if (last_process_status_change != prev) {
    struct process *p;
    for (p = process_list; p; p = p->next)
      if (p->have_status) {
//        … Examine p->status …
      }
  }
}

