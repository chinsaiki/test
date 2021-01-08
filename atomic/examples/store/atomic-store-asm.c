#include <stdint.h>
//http://preshing.com/20130618/atomic-vs-non-atomic-operations/
//https://gcc.gnu.org/wiki/Atomic/GCCMM/Optimizations

/*
    [rongtao@localhost store]$ gcc -O3 -m64 -S store-nonatomic.c 
    [rongtao@localhost store]$ cat store-nonatomic.s 
        ...
    	movabsq	$4294967298, %rax
    	movq	%rax, var(%rip)
    	ret
    	...

    [rongtao@localhost store]$ gcc -O3 -m64 -S store-nonatomic.c -DATOMIC
    [rongtao@localhost store]$ cat store-nonatomic.s
        ...
    	movabsq	$4294967298, %rax
    	movq	%rax, var(%rip)
    	ret
    	...

    
    [rongtao@localhost store]$ gcc -O3 -m32 -S store-nonatomic.c
    [rongtao@localhost store]$ cat store-nonatomic.s 
    	...
    	movl	$2, var
    	movl	$1, var+4
    	ret
    	...

    
    [rongtao@localhost store]$ gcc -O3 -m32 -S store-nonatomic.c -DATOMIC
    [rongtao@localhost store]$ cat store-nonatomic.s 
        ...
        subl    $12, %esp
        .cfi_def_cfa_offset 16
        movl    $2, %eax
        movl    $1, %edx
        movl    %eax, (%esp)
        movl    %edx, 4(%esp)
        fildq   (%esp)
        fistpq  var
        addl    $12, %esp
        .cfi_def_cfa_offset 4
        ret
        ...
        
*/
#ifdef VOLATILE
volatile
#endif
uint64_t var= 0;

void store()
{
#ifdef ATOMIC
    __atomic_store_n(&var, 0x100000002, __ATOMIC_RELAXED);
#else
    var= 0x100000002;
#endif
}

int main() 
{
    store();
}
