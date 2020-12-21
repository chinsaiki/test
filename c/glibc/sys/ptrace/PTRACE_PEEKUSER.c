#include <sys/ptrace.h>
#include <sys/user.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
 
int main(int argc, char *argv[])
{
    pid_t child;
    int status;
    struct user User;
    int orig_rax;
    int i;
 
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
 
        while (1) {
            /* 发送 PTRACE_SYSCALL 命令给被跟踪进程 (调用系统调用前，可以获取系统调用的参数) */
            ptrace(PTRACE_SYSCALL, child, NULL, NULL);

            /* 接收被子进程发送过来的 SIGCHLD 信号 */
            wait(&status); 
            /* 如果子进程退出了, 那么终止跟踪 */
            if(WIFEXITED(status)) { 
                break;
            }
#if 1
            /* 获取被跟踪进程寄存器的值 */
            ptrace(PTRACE_GETREGS, child, 0, &regs); 
#else
            char *pbyte = (char*)&User;
            for(i=0;i<sizeof(struct user);i++) {
                //这里需要知道 addr 地址
                ptrace(PTRACE_PEEKUSER, child, addr, pbyte); 
            }
#endif
            /* 获取rax寄存器的值 */
            orig_rax = User.regs.orig_rax; 

            /* 打印rax寄存器的值 */
            printf("syscall: orig_rax = %d()\n", orig_rax); 
 
            /* 发送 PTRACE_SYSCALL 命令给被跟踪进程 (调用系统调用后，可以获取系统调用的返回值) */
            ptrace(PTRACE_SYSCALL, child, NULL, NULL);

            /* 接收被子进程发送过来的 SIGCHLD 信号 */
            wait(&status); 
            
            /* 如果子进程退出了, 那么终止跟踪 */
            if(WIFEXITED(status)) { 
                break;
            }
        }
    }
 
    return 0;
}


