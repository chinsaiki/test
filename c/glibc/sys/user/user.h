/* Copyright (C) 2001-2012 Free Software Foundation, Inc.
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

#ifndef _SYS_USER_H
#define _SYS_USER_H	1

/* The whole purpose of this file is for GDB and GDB only.  Don't read
   too much into it.  Don't use it for anything other than GDB unless
   you know what you are doing.  */

#ifdef __x86_64__

struct user_fpregs_struct
{
  unsigned short int	cwd;
  unsigned short int	swd;
  unsigned short int	ftw;
  unsigned short int	fop;
  __extension__ unsigned long long int rip;
  __extension__ unsigned long long int rdp;
  unsigned int		mxcsr;
  unsigned int		mxcr_mask;
  unsigned int		st_space[32];   /* 8*16 bytes for each FP-reg = 128 bytes */
  unsigned int		xmm_space[64];  /* 16*16 bytes for each XMM-reg = 256 bytes */
  unsigned int		padding[24];
};

struct user_regs_struct
{
  __extension__ unsigned long long int r15;
  __extension__ unsigned long long int r14;
  __extension__ unsigned long long int r13;
  __extension__ unsigned long long int r12;
  __extension__ unsigned long long int rbp;
  __extension__ unsigned long long int rbx;
  __extension__ unsigned long long int r11;
  __extension__ unsigned long long int r10;
  __extension__ unsigned long long int r9;
  __extension__ unsigned long long int r8;
  __extension__ unsigned long long int rax;
  __extension__ unsigned long long int rcx;
  __extension__ unsigned long long int rdx;
  __extension__ unsigned long long int rsi;
  __extension__ unsigned long long int rdi;
  __extension__ unsigned long long int orig_rax;
  __extension__ unsigned long long int rip;
  __extension__ unsigned long long int cs;
  __extension__ unsigned long long int eflags;
  __extension__ unsigned long long int rsp;
  __extension__ unsigned long long int ss;
  __extension__ unsigned long long int fs_base;
  __extension__ unsigned long long int gs_base;
  __extension__ unsigned long long int ds;
  __extension__ unsigned long long int es;
  __extension__ unsigned long long int fs;
  __extension__ unsigned long long int gs;
};

struct user
{
  struct user_regs_struct	regs;
  int				u_fpvalid;
  struct user_fpregs_struct	i387;
  __extension__ unsigned long long int	u_tsize;
  __extension__ unsigned long long int	u_dsize;
  __extension__ unsigned long long int	u_ssize;
  __extension__ unsigned long long int	start_code;
  __extension__ unsigned long long int	start_stack;
  __extension__ long long int		signal;
  int				reserved;
  __extension__ union
    {
      struct user_regs_struct*	u_ar0;
      __extension__ unsigned long long int	__u_ar0_word;
    };
  __extension__ union
    {
      struct user_fpregs_struct*	u_fpstate;
      __extension__ unsigned long long int	__u_fpstate_word;
    };
  __extension__ unsigned long long int	magic;
  char				u_comm [32];
  __extension__ unsigned long long int	u_debugreg [8];
};

/* 荣涛 2020年12月21日

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

#else
/* These are the 32-bit x86 structures.  */
struct user_fpregs_struct
{
  long int cwd;
  long int swd;
  long int twd;
  long int fip;
  long int fcs;
  long int foo;
  long int fos;
  long int st_space [20];
};

struct user_fpxregs_struct
{
  unsigned short int cwd;
  unsigned short int swd;
  unsigned short int twd;
  unsigned short int fop;
  long int fip;
  long int fcs;
  long int foo;
  long int fos;
  long int mxcsr;
  long int reserved;
  long int st_space[32];   /* 8*16 bytes for each FP-reg = 128 bytes */
  long int xmm_space[32];  /* 8*16 bytes for each XMM-reg = 128 bytes */
  long int padding[56];
};

struct user_regs_struct
{
  long int ebx;
  long int ecx;
  long int edx;
  long int esi;
  long int edi;
  long int ebp;
  long int eax;
  long int xds;
  long int xes;
  long int xfs;
  long int xgs;
  long int orig_eax;
  long int eip;
  long int xcs;
  long int eflags;
  long int esp;
  long int xss;
};

struct user
{
  struct user_regs_struct	regs;
  int				u_fpvalid;
  struct user_fpregs_struct	i387;
  unsigned long int		u_tsize;
  unsigned long int		u_dsize;
  unsigned long int		u_ssize;
  unsigned long int		start_code;
  unsigned long int		start_stack;
  long int			signal;
  int				reserved;
  struct user_regs_struct*	u_ar0;
  struct user_fpregs_struct*	u_fpstate;
  unsigned long int		magic;
  char				u_comm [32];
  int				u_debugreg [8];
};
#endif  /* __x86_64__ */

#define PAGE_SHIFT		12
#define PAGE_SIZE		(1UL << PAGE_SHIFT)
#define PAGE_MASK		(~(PAGE_SIZE-1))
#define NBPG			PAGE_SIZE
#define UPAGES			1
#define HOST_TEXT_START_ADDR	(u.start_code)
#define HOST_STACK_END_ADDR	(u.start_stack + u.u_ssize * NBPG)

#endif	/* _SYS_USER_H */
