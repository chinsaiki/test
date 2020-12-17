/* Signal number definitions.  Linux version.
   Copyright (C) 1995,1996,1997,1998,1999,2003 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#ifdef	_SIGNAL_H

/* Fake signal functions.  */
#define SIG_ERR	((__sighandler_t) -1)		/* Error return.  */
#define SIG_DFL	((__sighandler_t) 0)		/* Default action.  */
#define SIG_IGN	((__sighandler_t) 1)		/* Ignore signal.  */

#ifdef __USE_UNIX98
# define SIG_HOLD	((__sighandler_t) 2)	/* Add signal to hold mask.  */
#endif

#if 0
SIGHUP 终止进程 终端线路挂断
SIGINT 终止进程 中断进程
SIGQUIT 建立baiCORE文件终止进程，并且生成core文件
SIGILL 建立CORE文件 非法指令
SIGTRAP 建立CORE文件 跟踪自陷
SIGBUS 建立CORE文件 总线错误
SIGSEGV 建立CORE文件 段非法错误
SIGFPE 建立CORE文件 浮点异常
SIGIOT 建立CORE文件 执行I/O自陷
SIGKILL 终止进程 杀死进程
SIGPIPE 终止进程 向一个没有读进程的管道写数据
SIGALARM 终止进程 计时器到时
SIGTERM 终止进程 软件终止信号
SIGSTOP 停止进程 非终端来的停止信号
SIGTSTP 停止进程 终端来的停止信号
SIGCONT 忽略信号 继续执行一个停止的进程
SIGURG 忽略信号 I/O紧急信号
SIGIO 忽略信号 描述符上可以进行I/O
SIGCHLD 忽略信号 当子进程停止或退出时通知父进程
SIGTTOU 停止进程 后台进程写终端
SIGTTIN 停止进程 后台进程读终端
SIGXGPU 终止进程 CPU时限超时
SIGXFSZ 终止进程 文件长度过长
SIGWINCH 忽略信号 窗口大小发生变化
SIGPROF 终止进程 统计分布图用计时器到时
SIGUSR1 终止进程 用户定义信号1
SIGUSR2 终止进程 用户定义信号2
SIGVTALRM 终止进程 虚拟计时器到时
#endif

/* Signals.  */
#define	SIGHUP		1	/* Hangup (POSIX).  */
#define	SIGINT		2	/* Interrupt (ANSI).  */
#define	SIGQUIT		3	/* Quit (POSIX).  */
#define	SIGILL		4	/* Illegal instruction (ANSI).  */
#define	SIGTRAP		5	/* Trace trap (POSIX).  */
#define	SIGABRT		6	/* Abort (ANSI).  */
#define	SIGIOT		6	/* IOT trap (4.2 BSD).  */
#define	SIGBUS		7	/* BUS error (4.2 BSD).  */
#define	SIGFPE		8	/* Floating-point exception (ANSI).  */
#define	SIGKILL		9	/* Kill, unblockable (POSIX).  */
#define	SIGUSR1		10	/* User-defined signal 1 (POSIX).  */
#define	SIGSEGV		11	/* Segmentation violation (ANSI).  */
#define	SIGUSR2		12	/* User-defined signal 2 (POSIX).  */
#define	SIGPIPE		13	/* Broken pipe (POSIX).  */
#define	SIGALRM		14	/* Alarm clock (POSIX).  */
#define	SIGTERM		15	/* Termination (ANSI).  */
#define	SIGSTKFLT	16	/* Stack fault.  */
#define	SIGCLD		SIGCHLD	/* Same as SIGCHLD (System V).  */
#define	SIGCHLD		17	/* Child status has changed (POSIX).  */
#define	SIGCONT		18	/* Continue (POSIX).  */
#define	SIGSTOP		19	/* Stop, unblockable (POSIX).  */
#define	SIGTSTP		20	/* Keyboard stop (POSIX).  */
#define	SIGTTIN		21	/* Background read from tty (POSIX).  */
#define	SIGTTOU		22	/* Background write to tty (POSIX).  */
#define	SIGURG		23	/* Urgent condition on socket (4.2 BSD).  */
#define	SIGXCPU		24	/* CPU limit exceeded (4.2 BSD).  */
#define	SIGXFSZ		25	/* File size limit exceeded (4.2 BSD).  */
#define	SIGVTALRM	26	/* Virtual alarm clock (4.2 BSD).  */
#define	SIGPROF		27	/* Profiling alarm clock (4.2 BSD).  */
#define	SIGWINCH	28	/* Window size change (4.3 BSD, Sun).  */
#define	SIGPOLL		SIGIO	/* Pollable event occurred (System V).  */
#define	SIGIO		29	/* I/O now possible (4.2 BSD).  */
#define	SIGPWR		30	/* Power failure restart (System V).  */
#define SIGSYS		31	/* Bad system call.  */
#define SIGUNUSED	31

#define	_NSIG		65	/* Biggest signal number + 1
				   (including real-time signals).  */

#define SIGRTMIN        (__libc_current_sigrtmin ())
#define SIGRTMAX        (__libc_current_sigrtmax ())

/* These are the hard limits of the kernel.  These values should not be
   used directly at user level.  */
#define __SIGRTMIN	32
#define __SIGRTMAX	(_NSIG - 1)

#endif	/* <signal.h> included.  */
