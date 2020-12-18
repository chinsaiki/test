#include "ptrace.h"
#include "anonyexec.h"
#include "elfreader.h"
#include "common.h"

//execve2(argv[0], argv, NULL);
int execve2(char *path, char *argv[], char *envp[])
{
    
    debug("execve2 start.\r\n");
    
	pid_t  child = 0;
	long   addr  = 0, argaddr = 0;
	int    status = 0, i = 0, arc = 0, arglen = 0;
	struct user_regs_struct regs;
	union
	{
		long val;
		char chars[sizeof(long)];
    } data;

    /* 虚拟地址起点 */
	uint64_t entry = elfentry(path);    //_start: entry point 

	child = fork();
	IFMSG(child == -1, 0, "fork");

    /* 子进程 */
	IF(child == 0, proc_child(path, argv, envp));
//    proc_child(path, argv, envp);
	debug("child pid = %d\r\n", child);
	while(1)
	{
	    /* 等待子进程的 信号  */
		wait(&status);
		if(WIFEXITED(status)) {
            debug("Start to ptrace... status = %d\r\n", status);
			break;
        }
        /* Get all general purpose registers used by a processes.
           This is not supported on all machines. 获取寄存器 */
		ptrace(PTRACE_GETREGS, child, NULL, &regs);
		if(regs.rip == entry)
		{
			MSG("EIP: _start %llx \r\n", regs.rip);
			MSG("RSP: %llx\r\n", regs.rsp);
			MSG("RSP + 8 => RDX(char **ubp_av) to __libc_start_main\r\n");
			//解析堆栈数据，栈顶为int argc
			addr = regs.rsp;
            /* Return the word in the process's text space at address ADDR. 返回 text 的 words 数 */
			arc = ptrace(PTRACE_PEEKTEXT, child, addr, NULL);
			MSG("argc: %d\r\n", arc);
			//POP ESI后栈顶为char **ubp_av, 同时可见此指针数组存储在堆栈之上
			addr += 8;
			//开始解析和修改参数
			for(i = 1;i < arc;i ++)
			{
				argaddr = ptrace(PTRACE_PEEKTEXT, child, addr + (i * sizeof(void*)), NULL);
				char *tmpstr = argv[i];
				int offset = 0;
				arglen = strlen(tmpstr);
				for(offset = 0; offset <= arglen; offset += sizeof(long))
				//读取参数字符串
				{
					data.val = ptrace(PTRACE_PEEKTEXT, child, argaddr, NULL);
					MSG("src: ubp_av[%d]: %s\r\n", i, data.chars);
					MSG("dst: upb_av[%d]: %s\r\n", i, argv[i]);
					strncpy(data.chars, tmpstr, sizeof(long));
                    debug("tmpstr = %s\n", tmpstr);
                    
                    /* Write the word DATA into the process's text space at address ADDR.  */
					ptrace(PTRACE_POKETEXT, child, argaddr, data.val);
					tmpstr += sizeof(long);
					argaddr += sizeof(long);
				}
			}
			ptrace(PTRACE_CONT, child, NULL, NULL); //继续执行进程
			ptrace(PTRACE_DETACH, child, NULL, NULL); //拆卸进程
            debug("break to ptrace... status = %d\r\n", status);
			break;
		}
        /* Single step the process.
            This is not supported on all machines.  */
		ptrace(PTRACE_SINGLESTEP, child, NULL, NULL);
//        debug("while done... status = %d\r\n", status);
	}
    debug("return... status = %d\r\n", status);
	return 0;
}

//加密 arg
static char *encryptedarg = "3abb6677af34ac57c0ca5828fd94f9d886c"
"26ce59a8ce60ecf6778079423dccff1d6f19cb655805d56098e6d38a1a710dee59523"
"eed7511e5a9e4b8ccb3a4686";

int proc_child(const char *path, char *argv[], char *envp[])
{
	int i = 1;

    /* 跟踪这个进程 */
    /* Indicate that the process making this request should be traced.
     All signals received by this process can be intercepted by its
     parent, and its parent can use the other `ptrace' requests.  */
	ptrace(PTRACE_TRACEME, 0, NULL, NULL);
	for(i = 1;argv[i] != NULL;i ++)
		argv[i] = encryptedarg;  //加密 arg
    
    debug("anonyexec start.\n");
	anonyexec(path, argv, envp);
    debug("anonyexec return.\n");
	return 0;
}
