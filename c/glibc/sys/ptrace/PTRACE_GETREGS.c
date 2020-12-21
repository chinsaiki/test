#include <sys/ptrace.h>
#include <sys/user.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <elf.h>
#include <stdint.h>

#include "common.h"

/* Get all general purpose registers used by a processes.
 This is not supported on all machines.  */
//PTRACE_GETREGS = 12, //<Intel386特有> 读取寄存器
//                   /*  ptrace(PTRACE_GETREGS, pid, 0, data);
//                        此功能将读取所有 17 个 基本寄存器的值
//                    */

int main(int argc, char *argv[])
{
    pid_t child;
    int status;
    struct user_regs_struct regs;
    
    
    child = fork();
    /* 子进程 */
    if (child == 0) {
        /* 本进程将被父进程跟踪，其父进程应该希望跟踪子进程 */
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("/bin/ls", "/bin/ls", NULL);
        exit(0);

    /* 父进程 */
    } else {
        /* 接收被子进程发送过来的 SIGCHLD 信号 */
        wait(&status); 
        /* 如果子进程退出了, 那么终止跟踪 */
        if(WIFEXITED(status)) { 
            return;
        }
        /* 获取被跟踪进程寄存器的值 */
        ptrace(PTRACE_GETREGS, child, 0, &regs); 
        perror("ptrace: ");
        
        /* 打印值 */
        print_user_regs_struct(&regs);
    }
 
    return 0;
}



