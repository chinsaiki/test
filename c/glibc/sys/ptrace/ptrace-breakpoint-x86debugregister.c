/**
 *  x86 调试寄存器设置断点
 *  
 *  作者：荣涛
 *  日期：2020年12月21日
 *
 */
/*
    x86 提供的8个调试寄存器(DR0-DR7)

    DR0-DR3: 存放断点地址寄存器
    DR4,DR5: 保留寄存器，不使用
    DR6:     状态寄存器
    DR7:     控制寄存器，标志位如下：
    ----------------------------------------------------------
                                         | Type | Length |
                                         |              /
                                         |             /
                                         |            /
                                         |           /
                                         |          /
                                         |         /
                                         |        /
                                         |       /
                                         |      /
                                         |     |
                                          16 18 20 22 24 26 28 30
         0 1 2 3 4 5 6 7       8-15       17 19 21 23 25 27 29 31
        |L|G|L|G|L|G|L|G|                |T |L |T |L |T |L |T |L |
        +--------------------------------------------------------+
        |D|D|D|D|D|D|D|D|               |D |D |D |D |D |D |D |D |
        |R|R|R|R|R|R|R|R|                |R |R |R |R |R |R |R |R |
        |0|0|1|1|2|2|3|3|                |0 |0 |1 |1 |2 |2 |3 |3 |
             是否激活                       Type: 断点类型 (00 execute, 01 write, 11 read-write)
                                        Length: 断点长度 (00 1-byte, 01 2-bytes, 11 4-bytes)

*/
#include <stdio.h>
#include <stddef.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/prctl.h>
#include <stdint.h>
#include <errno.h>
#include <stdbool.h>

extern int errno;

enum {
    BREAK_LOCAL = 0x1,
    BREAK_GLOBAL= 0x10,
};

enum {
	BREAK_EXEC = 0x0,
	BREAK_WRITE = 0x1,
	BREAK_RDWR = 0x3,
};

enum {
	BREAK_BYTE = 0x0,
	BREAK_WORD = 0x1,
	BREAK_DWORD = 0x3,
};

#define ENABLE_BREAKPOINT(x) (BREAK_LOCAL<<(x*2))

#define BREAK_TYPE_EXEC(x) (BREAK_EXEC<<(16+(x*4)))
#define BREAK_TYPE_WRITE(x) (BREAK_WRITE<<(16+(x*4)))
#define BREAK_TYPE_RDWR(x) (BREAK_RDWR<<(16+(x*4)))

#define BREAK_LEN_BYTE(x) (BREAK_BYTE<<(18+(x*4)))
#define BREAK_LEN_WORD(x) (BREAK_WORD<<(18+(x*4)))
#define BREAK_LEN_DWORD(x) (BREAK_DWORD<<(18+(x*4)))

/*
 * This function fork()s a child that will use
 * ptrace to set a hardware breakpoint for 
 * memory r/w at _addr_. When the breakpoint is
 * hit, then _handler_ is invoked in a signal-
 * handling context.
 */
bool set_breakpoint(void *addr, int bpno, void (*handler)(int)) {
	pid_t child = 0;
	uint32_t enable_breakpoint = ENABLE_BREAKPOINT(bpno);
	uint32_t enable_breakwrite = BREAK_TYPE_WRITE(bpno);
    uint32_t break_len = BREAK_LEN_BYTE(bpno);
	pid_t parent = getpid();
	int child_status = 0;
    
	if (!(child = fork())) //子进程
	{
		int parent_status = 0;
		if (ptrace(PTRACE_ATTACH, parent, NULL, NULL))
			_exit(1);

		while (!WIFSTOPPED(parent_status))
			waitpid(parent, &parent_status, 0);
	
		/*
		 * set the breakpoint address.
		 */
		if (ptrace(PTRACE_POKEUSER,
		           parent,
		           offsetof(struct user, u_debugreg[bpno]),
		           addr))
			_exit(1);

		/*
		 * set parameters for when the breakpoint should be triggered.
		 */
		if (ptrace(PTRACE_POKEUSER,
		           parent,
		           offsetof(struct user, u_debugreg[7]),
		           enable_breakwrite | enable_breakpoint | break_len))
			_exit(1);

		if (ptrace(PTRACE_DETACH, parent, NULL, NULL))
			_exit(1);

		_exit(0);
	}

	waitpid(child, &child_status, 0);

    //---------------------------------------------------------------------------------------------
    //子进程断点，处理子进程的 SIGTRAP 信号
	signal(SIGTRAP, handler);

	if (WIFEXITED(child_status) && !WEXITSTATUS(child_status))
		return true;
	return false;
}

/*
 * This function will disable a breakpoint by
 * invoking set_breakpoint is a 0x0 _addr_
 * and no handler function. See comments above
 * for implementation details.
 */
bool del_breakpoint(int bpno) 
{
	return set_breakpoint(0x0, bpno, NULL);
}

/*
 * Example of how to use this /library/.
 */
int handled_sigtrap = 0;

void handle_sigtrap(int s) {
	handled_sigtrap += 1;
	return;
}

int main(int argc, char **argv) {
	int i;
    char a[3] = {0};

	if (!set_breakpoint(a, 0, handle_sigtrap))
		printf("failed to set the breakpoint!\n");

    for(i=0;i<12321;i++) {
        a[0] = 1;
        a[1] = 1;
        a[2] = 1;
    }

	printf("handled_sigtrap: %d\n", handled_sigtrap);

	if (!del_breakpoint(0))
		printf("failed to disable the breakpoint!\n");



	return 1;
}


