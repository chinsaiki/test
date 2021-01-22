#include <stdio.h>

struct ctx;

struct ctx_ops {
    void (*init)(struct ctx *ctx, int value);
};
struct ctx {
    int value;
    struct ctx_ops *ops;
};


void ctx_init(struct ctx *cx, int value)
{
    cx->value = value;
}

struct ctx_ops ops = {
    .init = ctx_init,
};

int main()
{
    struct ctx c;
    c.ops = &ops;

#ifdef RAW    
//    main:
//        pushq   %rbp
//        movq    %rsp, %rbp
//        subq    $16, %rsp
//        movq    $ops, -8(%rbp)
//    
//        
//        leaq    -16(%rbp), %rax
//        movl    $12, %esi
//        movq    %rax, %rdi
//        call    ctx_init
//        leave
//        ret
    ctx_init(&c, 12);
#else
//    main:
//        pushq   %rbp
//        movq    %rsp, %rbp
//        subq    $16, %rsp
//        movq    $ops, -8(%rbp)
//        movq    -8(%rbp), %rax    ++++++++++++++++++多执行
//        movq    (%rax), %rax      ++++++++++++++++++多执行
//        leaq    -16(%rbp), %rdx
//        movl    $12, %esi
//        movq    %rdx, %rdi
//        call    *%rax
//        leave
//        ret
    c.ops->init(&c, 12);
#endif
    
}
