#include <stdio.h>

#ifdef INLINE
#define force_inline __attribute__ ((__always_inline__))
#else
#define force_inline
#endif

force_inline void f3(){
    int a = 1+3;
}

force_inline void f2(){
    f3();
}

force_inline void f1(){
    f2();
}

int main()
{
#ifdef CALL
//    f3:
//        pushq   %rbp
//        movq    %rsp, %rbp
//        movl    $4, -4(%rbp)
//        popq    %rbp
//        ret
//    f2:
//        pushq   %rbp
//        movq    %rsp, %rbp
//        movl    $0, %eax
//        call    f3
//        popq    %rbp
//        ret
//    f1:
//        pushq   %rbp
//        movq    %rsp, %rbp
//        movl    $0, %eax
//        call    f2
//        popq    %rbp
//        ret
//    main:
//        pushq   %rbp
//        movq    %rsp, %rbp
//        movl    $0, %eax
//        call    f1
//        popq    %rbp
//        ret
    f1();
#else
//    main:
//        pushq   %rbp
//        movq    %rsp, %rbp
//        movl    $0, %eax
//        call    f3
//        popq    %rbp
//        ret
    f3();
#endif
}

