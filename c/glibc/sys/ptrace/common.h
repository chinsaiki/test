#ifndef __ptrace_common_h
#define __ptrace_common_h 1

#include <stdio.h>
#include <sys/user.h>


#define debug(fmt...) do{fprintf(stderr, "[%s:%s %d]", __FILE__, __func__, __LINE__);fprintf(stderr, fmt);}while(0)


#ifdef __x86_64__

static void print_user_regs_struct(struct user_regs_struct *regs)
{
    //struct user_regs_struct
    //{
    //  __extension__ unsigned long long int r15;
    //  __extension__ unsigned long long int r14;
    //  __extension__ unsigned long long int r13;
    //  __extension__ unsigned long long int r12;
    //  __extension__ unsigned long long int rbp;
    //  __extension__ unsigned long long int rbx;
    //  __extension__ unsigned long long int r11;
    //  __extension__ unsigned long long int r10;
    //  __extension__ unsigned long long int r9;
    //  __extension__ unsigned long long int r8;
    //  __extension__ unsigned long long int rax;
    //  __extension__ unsigned long long int rcx;
    //  __extension__ unsigned long long int rdx;
    //  __extension__ unsigned long long int rsi;
    //  __extension__ unsigned long long int rdi;
    //  __extension__ unsigned long long int orig_rax;
    //  __extension__ unsigned long long int rip;
    //  __extension__ unsigned long long int cs;
    //  __extension__ unsigned long long int eflags;
    //  __extension__ unsigned long long int rsp;
    //  __extension__ unsigned long long int ss;
    //  __extension__ unsigned long long int fs_base;
    //  __extension__ unsigned long long int gs_base;
    //  __extension__ unsigned long long int ds;
    //  __extension__ unsigned long long int es;
    //  __extension__ unsigned long long int fs;
    //  __extension__ unsigned long long int gs;
    //};

    printf("r15     = 0x%016x\n", regs->r15); 
    printf("r14     = 0x%016x\n", regs->r14); 
    printf("r13     = 0x%016x\n", regs->r13); 
    printf("r12     = 0x%016x\n", regs->r12); 
    printf("rbp     = 0x%016x\n", regs->rbp); 
    printf("rbx     = 0x%016x\n", regs->rbx); 
    printf("r11     = 0x%016x\n", regs->r11); 
    printf("r10     = 0x%016x\n", regs->r10); 
    printf("r9      = 0x%016x\n", regs->r9); 
    printf("r8      = 0x%016x\n", regs->r8); 
    printf("rax     = 0x%016x\n", regs->rax); 
    printf("rcx     = 0x%016x\n", regs->rcx); 
    printf("rdx     = 0x%016x\n", regs->rdx); 
    printf("rsi     = 0x%016x\n", regs->rsi); 
    printf("rdi     = 0x%016x\n", regs->rdi); 
    printf("orig_rax = 0x%016x\n", regs->orig_rax); 
    printf("rip     = 0x%016x\n", regs->rip); 
    printf("cs      = 0x%016x\n", regs->cs); 
    printf("eflags  = 0x%016x\n", regs->eflags); 
    printf("rsp     = 0x%016x\n", regs->rsp); 
    printf("ss      = 0x%016x\n", regs->ss); 
    printf("fs_base = 0x%016x\n", regs->fs_base); 
    printf("gs_base = 0x%016x\n", regs->gs_base); 
    printf("ds      = 0x%016x\n", regs->ds); 
    printf("es      = 0x%016x\n", regs->es); 
    printf("fs      = 0x%016x\n", regs->fs); 
    printf("gs      = 0x%016x\n", regs->gs); 

}

#else  //__x86_64__

#error "Only support X86-64 arch linux OS."

#endif  //__x86_64__

#endif /*<__ptrace_common_h>*/
