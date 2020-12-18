#include <stdio.h>
#include <wait.h>
#include <syscall.h>
#include <unistd.h>
#include <signal.h>
#include <sched.h>
#include <sys/user.h>
#include <sys/ptrace.h>
#include <linux/ptrace.h>

/*

    打印出进程的所有的 系统调用 
    
204394  21 access 235
204394   2 open 236
204394   2 open 237
204394   5 fstat 238
204394   5 fstat 239
204394   9 mmap 240
204394   9 mmap 241
204394   3 close 242
204394   3 close 243
204394  16 ioctl 244
204394  16 ioctl 245
204394  16 ioctl 246
204394  16 ioctl 247

*/

#define debug(fmt...) do{fprintf(stderr, "[%s:%s %d]", __FILE__, __func__, __LINE__);fprintf(stderr, fmt);}while(0)


struct __syscall_name {
    int  code;
    char *name;
} __syscall_table[] = {
    {0, "read"},
    {1, "write"},
    {2, "open"},
    {3, "close"},
    {4, "stat"},
    {5, "fstat"},
    {6, "lstat"},
    {7, "poll"},
    {8, "lseek"},
    {9, "mmap"},
    {10, "mprotect"},
    {11, "munmap"},
    {12, "brk"},
    {13, "rt_sigaction"},
    {14, "rt_sigprocmask"},
    {15, "rt_sigreturn"},
    {16, "ioctl"},
    {17, "pread64"},
    {18, "pwrite64"},
    {19, "readv"},
    {20, "writev"},
    {21, "access"},
    {22, "pipe"},
    {23, "select"},
    {24, "sched_yield"},
    {25, "mremap"},
    {26, "msync"},
    {27, "mincore"},
    {28, "madvise"},
    {29, "shmget"},
    {30, "shmat"},
    {31, "shmctl"},
    {32, "dup"},
    {33, "dup2"},
    {34, "pause"},
    {35, "nanosleep"},
    {36, "getitimer"},
    {37, "alarm"},
    {38, "setitimer"},
    {39, "getpid"},
    {40, "sendfile"},
    {41, "socket"},
    {42, "connect"},
    {43, "accept"},
    {44, "sendto"},
    {45, "recvfrom"},
    {46, "sendmsg"},
    {47, "recvmsg"},
    {48, "shutdown"},
    {49, "bind"},
    {50, "listen"},
    {51, "getsockname"},
    {52, "getpeername"},
    {53, "socketpair"},
    {54, "setsockopt"},
    {55, "getsockopt"},
    {56, "clone"},
    {57, "fork"},
    {58, "vfork"},
    {59, "execve"},
    {60, "exit"},
    {61, "wait4"},
    {62, "kill"},
    {63, "uname"},
    {64, "semget"},
    {65, "semop"},
    {66, "semctl"},
    {67, "shmdt"},
    {68, "msgget"},
    {69, "msgsnd"},
    {70, "msgrcv"},
    {71, "msgctl"},
    {72, "fcntl"},
    {73, "flock"},
    {74, "fsync"},
    {75, "fdatasync"},
    {76, "truncate"},
    {77, "ftruncate"},
    {78, "getdents"},
    {79, "getcwd"},
    {80, "chdir"},
    {81, "fchdir"},
    {82, "rename"},
    {83, "mkdir"},
    {84, "rmdir"},
    {85, "creat"},
    {86, "link"},
    {87, "unlink"},
    {88, "symlink"},
    {89, "readlink"},
    {90, "chmod"},
    {91, "fchmod"},
    {92, "chown"},
    {93, "fchown"},
    {94, "lchown"},
    {95, "umask"},
    {96, "gettimeofday"},
    {97, "getrlimit"},
    {98, "getrusage"},
    {99, "sysinfo"},
    {100, "times"},
    {101, "ptrace"},
    {102, "getuid"},
    {103, "syslog"},
    {104, "getgid"},
    {105, "setuid"},
    {106, "setgid"},
    {107, "geteuid"},
    {108, "getegid"},
    {109, "setpgid"},
    {110, "getppid"},
    {111, "getpgrp"},
    {112, "setsid"},
    {113, "setreuid"},
    {114, "setregid"},
    {115, "getgroups"},
    {116, "setgroups"},
    {117, "setresuid"},
    {118, "getresuid"},
    {119, "setresgid"},
    {120, "getresgid"},
    {121, "getpgid"},
    {122, "setfsuid"},
    {123, "setfsgid"},
    {124, "getsid"},
    {125, "capget"},
    {126, "capset"},
    {127, "rt_sigpending"},
    {128, "rt_sigtimedwait"},
    {129, "rt_sigqueueinfo"},
    {130, "rt_sigsuspend"},
    {131, "sigaltstack"},
    {132, "utime"},
    {133, "mknod"},
    {134, "uselib"},
    {135, "personality"},
    {136, "ustat"},
    {137, "statfs"},
    {138, "fstatfs"},
    {139, "sysfs"},
    {140, "getpriority"},
    {141, "setpriority"},
    {142, "sched_setparam"},
    {143, "sched_getparam"},
    {144, "sched_setscheduler"},
    {145, "sched_getscheduler"},
    {146, "sched_get_priority_max"},
    {147, "sched_get_priority_min"},
    {148, "sched_rr_get_interval"},
    {149, "mlock"},
    {150, "munlock"},
    {151, "mlockall"},
    {152, "munlockall"},
    {153, "vhangup"},
    {154, "modify_ldt"},
    {155, "pivot_root"},
    {156, "_sysctl"},
    {157, "prctl"},
    {158, "arch_prctl"},
    {159, "adjtimex"},
    {160, "setrlimit"},
    {161, "chroot"},
    {162, "sync"},
    {163, "acct"},
    {164, "settimeofday"},
    {165, "mount"},
    {166, "umount2"},
    {167, "swapon"},
    {168, "swapoff"},
    {169, "reboot"},
    {170, "sethostname"},
    {171, "setdomainname"},
    {172, "iopl"},
    {173, "ioperm"},
    {174, "create_module"},
    {175, "init_module"},
    {176, "delete_module"},
    {177, "get_kernel_syms"},
    {178, "query_module"},
    {179, "quotactl"},
    {180, "nfsservctl"},
    {181, "getpmsg"},
    {182, "putpmsg"},
    {183, "afs_syscall"},
    {184, "tuxcall"},
    {185, "security"},
    {186, "gettid"},
    {187, "readahead"},
    {188, "setxattr"},
    {189, "lsetxattr"},
    {190, "fsetxattr"},
    {191, "getxattr"},
    {192, "lgetxattr"},
    {193, "fgetxattr"},
    {194, "listxattr"},
    {195, "llistxattr"},
    {196, "flistxattr"},
    {197, "removexattr"},
    {198, "lremovexattr"},
    {199, "fremovexattr"},
    {200, "tkill"},
    {201, "time"},
    {202, "futex"},
    {203, "sched_setaffinity"},
    {204, "sched_getaffinity"},
    {205, "set_thread_area"},
    {206, "io_setup"},
    {207, "io_destroy"},
    {208, "io_getevents"},
    {209, "io_submit"},
    {210, "io_cancel"},
    {211, "get_thread_area"},
    {212, "lookup_dcookie"},
    {213, "epoll_create"},
    {214, "epoll_ctl_old"},
    {215, "epoll_wait_old"},
    {216, "remap_file_pages"},
    {217, "getdents64"},
    {218, "set_tid_address"},
    {219, "restart_syscall"},
    {220, "semtimedop"},
    {221, "fadvise64"},
    {222, "timer_create"},
    {223, "timer_settime"},
    {224, "timer_gettime"},
    {225, "timer_getoverrun"},
    {226, "timer_delete"},
    {227, "clock_settime"},
    {228, "clock_gettime"},
    {229, "clock_getres"},
    {230, "clock_nanosleep"},
    {231, "exit_group"},
    {232, "epoll_wait"},
    {233, "epoll_ctl"},
    {234, "tgkill"},
    {235, "utimes"},
    {236, "vserver"},
    {237, "mbind"},
    {238, "set_mempolicy"},
    {239, "get_mempolicy"},
    {240, "mq_open"},
    {241, "mq_unlink"},
    {242, "mq_timedsend"},
    {243, "mq_timedreceive"},
    {244, "mq_notify"},
    {245, "mq_getsetattr"},
    {246, "kexec_load"},
    {247, "waitid"},
    {248, "add_key"},
    {249, "request_key"},
    {250, "keyctl"},
    {251, "ioprio_set"},
    {252, "ioprio_get"},
    {253, "inotify_init"},
    {254, "inotify_add_watch"},
    {255, "inotify_rm_watch"},
    {256, "migrate_pages"},
    {257, "openat"},
    {258, "mkdirat"},
    {259, "mknodat"},
    {260, "fchownat"},
    {261, "futimesat"},
    {262, "newfstatat"},
    {263, "unlinkat"},
    {264, "renameat"},
    {265, "linkat"},
    {266, "symlinkat"},
    {267, "readlinkat"},
    {268, "fchmodat"},
    {269, "faccessat"},
    {270, "pselect6"},
    {271, "ppoll"},
    {272, "unshare"},
    {273, "set_robust_list"},
    {274, "get_robust_list"},
    {275, "splice"},
    {276, "tee"},
    {277, "sync_file_range"},
    {278, "vmsplice"},
    {279, "move_pages"},
    {280, "utimensat"},
    {281, "epoll_pwait"},
    {282, "signalfd"},
    {283, "timerfd_create"},
    {284, "eventfd"},
    {285, "fallocate"},
    {286, "timerfd_settime"},
    {287, "timerfd_gettime"},
    {288, "accept4"},
    {289, "signalfd4"},
    {290, "eventfd2"},
    {291, "epoll_create1"},
    {292, "dup3"},
    {293, "pipe2"},
    {294, "inotify_init1"},
    {295, "preadv"},
    {296, "pwritev"},
    {297, "rt_tgsigqueueinfo"},
    {298, "perf_event_open"},
    {299, "recvmmsg"},
    {300, "fanotify_init"},
    {301, "fanotify_mark"},
    {302, "prlimit64"},
    {303, "name_to_handle_at"},
    {304, "open_by_handle_at"},
    {305, "clock_adjtime"},
    {306, "syncfs"},
    {307, "sendmmsg"},
    {308, "setns"},
    {309, "getcpu"},
    {310, "process_vm_readv"},
    {311, "process_vm_writev"},
    {312, "kcmp"},
    {313, "finit_module"},
    {314, "sched_setattr"},
    {315, "sched_getattr"},
    {316, "renameat2"},
    {317, "seccomp"},
    {318, "getrandom"},
    {319, "memfd_create"},
    {320, "kexec_file_load"},
    {321, "bpf"},
    {322, "execveat"},
    {323, "userfaultfd"},
    {324, "membarrier"},
    {325, "mlock2"},
    {326, "copy_file_range"},
    {327, "preadv2"},
    {328, "pwritev2"},
    {329, "pkey_mprotect"},
    {330, "pkey_alloc"},
    {331, "pkey_free"},
    {332, "statx"},
    {333, "io_pgetevents"},
    {334, "rseq"},
    {424, "pidfd_send_signal"},
    {425, "io_uring_setup"},
    {426, "io_uring_enter"},
    {427, "io_uring_register"},
    {428, "open_tree"},
    {429, "move_mount"},
    {430, "fsopen"},
    {431, "fsconfig"},
    {432, "fsmount"},
    {433, "fspick"},
    {434, "pidfd_open"},
    {435, "clone3"},
    {436, "close_range"},
    {437, "openat2"},
    {438, "pidfd_getfd"},
    {439, "faccessat2"},
    {440, "process_madvise"},
    {512, "rt_sigaction"},
    {513, "rt_sigreturn"},
    {514, "ioctl"},
    {515, "readv"},
    {516, "writev"},
    {517, "recvfrom"},
    {518, "sendmsg"},
    {519, "recvmsg"},
    {520, "execve"},
    {521, "ptrace"},
    {522, "rt_sigpending"},
    {523, "rt_sigtimedwait"},
    {524, "rt_sigqueueinfo"},
    {525, "sigaltstack"},
    {526, "timer_create"},
    {527, "mq_notify"},
    {528, "kexec_load"},
    {529, "waitid"},
    {530, "set_robust_list"},
    {531, "get_robust_list"},
    {532, "vmsplice"},
    {533, "move_pages"},
    {534, "preadv"},
    {535, "pwritev"},
    {536, "rt_tgsigqueueinfo"},
    {537, "recvmmsg"},
    {538, "sendmmsg"},
    {539, "process_vm_readv"},
    {540, "process_vm_writev"},
    {541, "setsockopt"},
    {542, "getsockopt"},
    {543, "io_setup"},
    {544, "io_submit"},
    {545, "execveat"},
    {546, "preadv2"},
    {547, "pwritev2"},
    {-1, NULL},
};
 
 
char *get_syscall_symbol(int code) {
    struct __syscall_name *sc;
 
    for (sc = __syscall_table; sc->code >= 0; sc++) {
        if (sc->code == code) {
            return sc->name;
        }
    }
 
    return NULL;
}


void print_registers(struct user_regs_struct * regs)
{
    printf("rdi : %llu\n", regs->rdi);
    printf("rsi : %llu\n", regs->rsi);
    printf("rdx : %llu\n", regs->rdx);
    printf("r10 : %llu\n", regs->r10);
    printf("r8 : %llu\n", regs->r8);
    printf("r9 : %llu\n", regs->r9);
}

void handle_syscall(pid_t pid)
{
    static int count = 0;
    ++count;
    struct user_regs_struct regs;

    ptrace(PTRACE_GETREGS, pid, NULL, &regs);
    
    printf("%8d %3llu %s %d\n", pid, regs.orig_rax, get_syscall_symbol(regs.orig_rax), count);

    ptrace(PTRACE_SYSCALL, pid, 0, 0);
}

void wait_for_procs()
{
    int status, signal, event;
    pid_t pid, new_pid;
    int count = 0;

    while (1)
    {
        pid = waitpid(-1, &status, __WALL); /* 等待所有的子进程 */
        if(pid == -1) {
            break;
        }

        signal = WSTOPSIG(status); /* 子进程的信号 */
        event = status >> 16;
        
        if(WIFEXITED(status)) { /* exit */
            ++ count;
            printf("count : %d, pid : %d\n", count, pid);
            if(count > 1) {
                printf("all is over: count : %d, pid : %d\n", count, pid);
                break;
            }
        } else if(WIFSTOPPED(status) && signal == (SIGTRAP | 0x80)) { /* stop */
            handle_syscall(pid);
            
        /*SIGTRAP 在大多数系统，gdb对使用fork创建的进程没有进行特别的支持。当父进程使用fork创建子进程
        ，gdb仍然只会调试父进程，而子进程没有得到控制和调试。这个时候，如果你在子进程执行到
        的代码中设置了断点，那么当子进程执行到这个断点的时候，会产生一个SIGTRAP的信号，
        如果没有对此信号进行捕捉处理，就会按默认的处理方式处理——终止进程。 */
        } else if(signal == SIGTRAP && (event == PTRACE_EVENT_FORK || /* 检测到是 子进程的 fork vfork clone  */
                event == PTRACE_EVENT_VFORK ||
                event == PTRACE_EVENT_CLONE)) {

            /* Get last ptrace message.  */
            ptrace(PTRACE_GETEVENTMSG, pid, 0, &new_pid);
            printf("new process, pid = %d\n", new_pid);

            ptrace(PTRACE_SETOPTIONS, new_pid, 0,
                    PTRACE_O_TRACESYSGOOD | PTRACE_O_TRACEFORK |
                    PTRACE_O_TRACECLONE | PTRACE_O_TRACEVFORK |
                    PTRACE_O_TRACEEXEC | PTRACE_O_EXITKILL);
            printf("attach the father(%d) : %ld\n", pid, ptrace(PTRACE_ATTACH, pid, 0, 0));
            
            pid_t pidd = waitpid(new_pid, NULL, WUNTRACED);
            printf("拦到的是：%d\n", pidd); 
            
            printf("free father process.");
            printf("%ld\n", ptrace(PTRACE_DETACH, pid, NULL, NULL));
            printf("attach father process.");
            printf("%ld\n", ptrace(PTRACE_ATTACH, pid, NULL, NULL));
            printf("start father process.\n");
            printf("%lu\n", ptrace(PTRACE_SYSCALL, pid, NULL, NULL));
            ptrace(PTRACE_SYSCALL, new_pid, NULL, NULL);
        } else if(signal == SIGTRAP && event == PTRACE_EVENT_EXEC) {  /* 子进程执行了 exec */
            printf("catch the pid : %d\n", pid);
            ptrace(PTRACE_SYSCALL, pid, 0, 0);
        } else {
            ptrace(PTRACE_SYSCALL, pid, 0, 0);
        }

        // ptrace(PTRACE_SYSCALL, pid, 0, 0);
    }
    
}

void ptrace3_setup(char *argv[])
{
    pid_t pid;
    int status;

    pid = fork();

    if(pid < 0) {
        perror("fork");
    } else if(pid == 0) {
        execv(argv[1], argv + 1);
    } else {
        ptrace(PTRACE_ATTACH, pid, NULL, NULL); /* attach 子进程 */
        waitpid(pid, &status, WUNTRACED); /* 报告子进程的状态 */

        ptrace(PTRACE_SETOPTIONS, pid, 0, 
            PTRACE_O_TRACEEXEC | PTRACE_O_TRACECLONE | PTRACE_O_TRACEFORK |
            PTRACE_O_TRACESYSGOOD | PTRACE_O_TRACEEXIT | PTRACE_O_TRACEVFORK); /* 为子进程设置 选项       options */
        
        ptrace(PTRACE_SYSCALL, pid, 0, 0);/* 当系统调用时 停止 */
        waitpid(pid, &status, WUNTRACED);/* 报告子进程状态 */

        ptrace(PTRACE_SYSCALL, pid, 0, 0); /* 检测系统调用 */
    }
    
}

int main(int argc, char *argv[])
{
    ptrace3_setup(argv);
     printf("Are you ok?\n");
    wait_for_procs();

    return 0;
}
