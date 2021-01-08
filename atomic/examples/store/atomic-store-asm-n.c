#include <stdint.h>
//http://preshing.com/20130618/atomic-vs-non-atomic-operations/
//https://gcc.gnu.org/wiki/Atomic/GCCMM/Optimizations

/*
    [rongtao@localhost store]$ gcc -O3 -S atomic-store-asm-n.c 
    [rongtao@localhost store]$ cat atomic-store-asm-n.s 
	...
    	movl	$1, stage(%rip)
    .L2:
    	jmp	.L2
	...

    
    [rongtao@localhost store]$ gcc -O3 -S atomic-store-asm-n.c -DATOMIC
    [rongtao@localhost store]$ cat atomic-store-asm-n.s 
    ...

    	movl	$1, stage(%rip)
    	.p2align 4,,10
    	.p2align 3
    .L3:
    	movl	stage(%rip), %eax
    	cmpl	$2, %eax
    	jne	.L3
    	movl	$3, stage(%rip)
    	ret
    	.cfi_endproc
    ...

*/
#ifdef VOLATILE
volatile
#endif
uint32_t stage= 0;
void thread1()
{
#ifdef ATOMIC
    __atomic_store_n(&stage, 1, __ATOMIC_RELAXED);
    while (__atomic_load_n(&stage, __ATOMIC_RELAXED) != 2);
    __atomic_store_n(&stage, 3, __ATOMIC_RELAXED);
#else
    stage= 1;
    while (stage != 2);
    stage= 3;
#endif
}


int main() 
{
    thread1();
}

