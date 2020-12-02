
//# gcc -S -c test.c

//main:
//  pushq %rbp
//  movq %rsp, %rbp
//  movl $0, %eax
//  leave
//  ret

//或者    gcc -S -c __sync_synchronize.c
//main:
//    pushq   %rbp
//    movq    %rsp, %rbp
//    mfence
//    movl    $0, %eax
//    popq    %rbp
//    ret

//或者    gcc -S -c __sync_synchronize.c -std=c11
//main:
//	pushq	%rbp
//	movq	%rsp, %rbp
//	mfence
//	movl	$0, %eax
//	popq	%rbp
//	ret

//或者    gcc -S -c __sync_synchronize.c -std=c99
//main:
//	pushq	%rbp
//	movq	%rsp, %rbp
//	mfence
//	movl	$0, %eax
//	popq	%rbp
//	ret

//mfence
//"Reads cannot pass earlier MFENCE instructions”
//“Writes cannot pass earlier MFENCE instructions. ”
//“MFENCE instructions cannot pass earlier reads or writes”



int main(){
  __sync_synchronize();

//  asm volatile ("" : : : "memory"); //不生成任何代码，只是给编译器看的
  
  return 0;
}

