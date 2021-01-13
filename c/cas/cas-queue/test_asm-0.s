enqueue_task:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%r12
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -40(%rbp)
	movl	$0, -20(%rbp)
.L4:
	movl	$1, %edx
	movl	$1, %eax
	lock; cmpxchgl %edx,test_queue(%rip); setz %al 	
	movb	%al, -21(%rbp)
	movzbl	-21(%rbp), %eax
	testb	%al, %al
	je	.L2
	rdtsc 
	movl	%eax, %r12d
	movl	%edx, %ebx
	movl	%r12d, %eax
	movl	%ebx, %edx
	salq	$32, %rdx
	addq	%rdx, %rax
	movq	%rax, latency(%rip)	--------------------------------------------------> 获取时间 起始点
	movl	$2, %edx	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	movl	$1, %eax	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	lock; cmpxchgl %edx,test_queue(%rip); setz %al+++++++++++++++++++++++++++++++++ 标记队列可读
	movb	%al, -22(%rbp)
	addl	$1, -20(%rbp)
	cmpl	$1048576, -20(%rbp)
	jne	.L2
	nop
	movl	$0, %edi
	call	pthread_exit
.L2:
	jmp	.L4
dequeue_task:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%r12
	pushq	%rbx
	subq	$48, %rsp
	movq	%rdi, -56(%rbp)
	movl	$0, -20(%rbp)
	movq	$0, -32(%rbp)
.L8:
	movl	$2, %edx
	movl	$2, %eax
	lock; cmpxchgl %edx,test_queue(%rip); setz %al ++++++++++++++++++++++++++++++++ 队列可读
	movb	%al, -33(%rbp)+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	movzbl	-33(%rbp), %eax++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	testb	%al, %al+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	je	.L6
	rdtsc++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	movl	%eax, %r12d++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	movl	%edx, %ebx+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	movl	%r12d, %eax++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	movl	%ebx, %edx+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	salq	$32, %rdx++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	addq	%rax, %rdx+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	movq	latency(%rip), %rax --------------------------------------------------> 获取时间 终止点
	subq	%rax, %rdx
	movq	%rdx, %rax
	addq	%rax, -32(%rbp)
	movq	$0, latency(%rip)   
	movl	$1, %edx
	movl	$2, %eax
	lock; cmpxchgl %edx,test_queue(%rip); setz %al -------------------------------> 标记队列可写
	movb	%al, -34(%rbp)
	addl	$1, -20(%rbp)
	cmpl	$1048576, -20(%rbp)
	jne	.L6
	nop
	movq	-32(%rbp), %rax
	testq	%rax, %rax
	js	.L9
	jmp	.L13
.L6:
	jmp	.L8
.L13:
	cvtsi2sdq	%rax, %xmm0
	jmp	.L10
.L9:
	movq	%rax, %rdx
	shrq	%rdx
	andl	$1, %eax
	orq	%rax, %rdx
	cvtsi2sdq	%rdx, %xmm0
	addsd	%xmm0, %xmm0
.L10:
	movsd	.LC0(%rip), %xmm1
	divsd	%xmm1, %xmm0
	movsd	.LC1(%rip), %xmm1
	divsd	%xmm1, %xmm0
	movsd	.LC2(%rip), %xmm1
	mulsd	%xmm0, %xmm1
	movq	-32(%rbp), %rax
	testq	%rax, %rax
	js	.L11
	cvtsi2sdq	%rax, %xmm0
	jmp	.L12
.L11:
	movq	%rax, %rdx
	shrq	%rdx
	andl	$1, %eax
	orq	%rax, %rdx
	cvtsi2sdq	%rdx, %xmm0
	addsd	%xmm0, %xmm0
.L12:
	movsd	.LC0(%rip), %xmm2
	divsd	%xmm2, %xmm0
	movl	$.LC3, %edi
	movl	$2, %eax
	call	printf
	movl	$0, %edi
	call	pthread_exit
	.cfi_endproc
.LFE3:
	.size	dequeue_task, .-dequeue_task
	.globl	main
	.type	main, @function
main:
.LFB4:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	leaq	-8(%rbp), %rax
	movl	$0, %ecx
	movl	$enqueue_task, %edx
	movl	$0, %esi
	movq	%rax, %rdi
	call	pthread_create
	leaq	-16(%rbp), %rax
	movl	$0, %ecx
	movl	$dequeue_task, %edx
	movl	$0, %esi
	movq	%rax, %rdi
	call	pthread_create
	movq	-8(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	pthread_join
	movq	-16(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	pthread_join
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	main, .-main
	.section	.rodata
	.align 8
.LC0:
	.long	0
	.long	1093664768
	.align 8
.LC1:
	.long	3221225472
	.long	1105615371
	.align 8
.LC2:
	.long	0
	.long	1104006501
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-39)"
	.section	.note.GNU-stack,"",@progbits
