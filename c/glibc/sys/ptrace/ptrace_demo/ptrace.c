#include <stdio.h>
#include <sys/ptrace.h>
#include <syscall.h>
#include <sys/user.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>
#include <assert.h>
#include <linux/ptrace.h>

/*
    程序介绍

    当 调试 的程序运行至 某些规定的 系统调用 时， 停止， 打印一条信息

203562 was stopped at 1, fork : 57, vfork : 58, clone : 56
heheh  2 203562 203561
203562 was stopped at 1, fork : 57, vfork : 58, clone : 56
203562 was stopped at 1, fork : 57, vfork : 58, clone : 56
Hello world!
203562 was stopped at 1, fork : 57, vfork : 58, clone : 56
203562 was stopped at 231, fork : 57, vfork : 58, clone : 56
*/

#define TRUE    1
#define FALSE   0
#define VAR_NUM 1

pid_t spid[VAR_NUM];

static void ptrace1_setup(char **argv)
{
    pid_t pid;
    int status;
    unsigned long long orig_rax;
    int i;
    
    for(i = 0; i < VAR_NUM; ++i) {
        pid = fork();
        if(pid < 0) {
            perror("fork");
        } else if(pid == 0) { //子进程执行 命令行指令
            execv(argv[1], argv + 1);
        } else { //父进程 接管 子进程 
            spid[i] = pid;
            ptrace(PTRACE_ATTACH, spid[i], NULL, NULL);
            pid = waitpid(spid[i], &status, WUNTRACED);
            assert(pid == spid[i]);
            ptrace(PTRACE_SETOPTIONS, spid[i], NULL,
                    PTRACE_O_TRACEEXEC | PTRACE_O_TRACEEXIT |
                    PTRACE_O_TRACESYSGOOD | PTRACE_O_EXITKILL); //检测系统调用 
            ptrace(PTRACE_SYSCALL, spid[i], NULL, NULL);
            waitpid(spid[i], &status, WUNTRACED);

            /* PTRACE_PEEKUSER ： Return the word in the process's user area at offset ADDR.  */
            orig_rax = ptrace(PTRACE_PEEKUSER, spid[i], 8 * 15, NULL);
            assert(orig_rax == __NR_execve);
        }
    }

    for(i = 0; i < VAR_NUM; ++i) {
        /* Continue and stop at the next (return from) syscall. 在下一次系统调用停止 */
        ptrace(PTRACE_SYSCALL, spid[i], NULL, NULL);
    }
}

static void wait_for_procs()
{
    int status, sig;
    pid_t pid;
    struct user_regs_struct regs;
    int i;
    
    // for(int count = 0; count < VAR_NUM; )
    int count = 0;
    while (TRUE)
    {
        // printf("%d\n", count);

        pid = waitpid(-1, &status, WUNTRACED);
        sig = WSTOPSIG(status);

        //count = (count + 1) % VAR_NUM;

        if(WIFEXITED(status)) {
            // break;
            ++ count;
            if(count == 2) {
                break;
            }
        } else if(WIFSTOPPED(status) && sig == (SIGTRAP | 0x80)) {
            ptrace(PTRACE_GETREGS, pid, NULL, &regs);
            printf("%d was stopped at %llu, fork : %d, vfork : %d, clone : %d\n", 
                        pid, regs.orig_rax, __NR_fork, __NR_vfork, __NR_clone);
        }
        
        if(count == 0) {
            // ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
            for(i = 0; i < VAR_NUM; ++i) {
                /* Continue and stop at the next (return from) syscall. 在下一次系统调用停止 */
                ptrace(PTRACE_SYSCALL, spid[i], NULL, NULL);
            }
        }
       
        /* Continue and stop at the next (return from) syscall. 在下一次系统调用停止 */
       ptrace(PTRACE_SYSCALL, spid[0], NULL, NULL);
    }
    
}

int main(int argc, char *argv[])
{
    ptrace1_setup(argv);
    wait_for_procs();

    return 0;
}
