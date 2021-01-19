enqueue_task:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%r12
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -40(%rbp)
	movl	$0, -20(%rbp)
	movl	$0, -20(%rbp)
	jmp	.L2
.L3:
	movl	-20(%rbp), %eax
	addl	$1, %eax
	movslq	%eax, %rdx
	movl	-20(%rbp), %eax
	cltq
	movq	%rdx, test_msgs.3221(,%rax,8)
	addl	$1, -20(%rbp)
.L2:
	movl	-20(%rbp), %eax
	cmpl	$1048575, %eax
	jbe	.L3
	movl	$0, -20(%rbp)
.L6:
	movl	$1, %edx
	movl	$1, %eax
	lock; cmpxchgl %edx,test_queue(%rip); setz %al
	movb	%al, -21(%rbp)
	movzbl	-21(%rbp), %eax
	testb	%al, %al
	je	.L4
	rdtsc
	movl	%eax, %r12d
	movl	%edx, %ebx
	movl	%r12d, %eax
	movl	%ebx, %edx
	salq	$32, %rdx
	addq	%rdx, %rax
	movq	%rax, latency(%rip)---------------------------------------------------> 获取时间 起始点
	movl	-20(%rbp), %eax++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	cltq
	salq	$3, %rax+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	addq	$test_msgs.3221, %rax++++++++++++++++++++++++++++++++++++++++++++++++++ 指向需要“传输”的指针
	movq	%rax, test_queue+8(%rip)+++++++++++++++++++++++++++++++++++++++++++++++
	movl	$2, %edx+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	movl	$1, %eax+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	lock; cmpxchgl %edx,test_queue(%rip); setz %al+++++++++++++++++++++++++++++++++ 标记队列可读
	movb	%al, -22(%rbp)
	addl	$1, -20(%rbp)
	cmpl	$1048576, -20(%rbp)
	jne	.L4
	nop
	movl	$0, %edi
	call	pthread_exit
.L4:
	jmp	.L6
dequeue_task:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%r12
	pushq	%rbx
	subq	$64, %rsp
	.cfi_offset 12, -24
	.cfi_offset 3, -32
	movq	%rdi, -72(%rbp)
	movl	$0, -20(%rbp)
	movq	$0, -32(%rbp)
.L10:
	movl	$2, %edx
	movl	$2, %eax
	lock; cmpxchgl %edx,test_queue(%rip); setz %al ++++++++++++++++++++++++++++++++ 队列可读
	movb	%al, -33(%rbp)+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	movzbl	-33(%rbp), %eax++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	testb	%al, %al+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	je	.L8
	movq	test_queue+8(%rip), %rax+++++++++++++++++++++++++++++++++++++++++++++++
	movq	%rax, -48(%rbp)++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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
	movb	%al, -49(%rbp)
	addl	$1, -20(%rbp)
	cmpl	$1048576, -20(%rbp)
	jne	.L8
	nop
	movq	-32(%rbp), %rax
	testq	%rax, %rax
	js	.L11
	jmp	.L15
.L8:
	jmp	.L10
.L15:
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
	movsd	.LC0(%rip), %xmm1
	divsd	%xmm1, %xmm0
	movsd	.LC1(%rip), %xmm1
	divsd	%xmm1, %xmm0
	movsd	.LC2(%rip), %xmm1
	mulsd	%xmm0, %xmm1
	movq	-32(%rbp), %rax
	testq	%rax, %rax
	js	.L13
	cvtsi2sdq	%rax, %xmm0
	jmp	.L14
.L13:
	movq	%rax, %rdx
	shrq	%rdx
	andl	$1, %eax
	orq	%rax, %rdx
	cvtsi2sdq	%rdx, %xmm0
	addsd	%xmm0, %xmm0
.L14:
	movsd	.LC0(%rip), %xmm2
	divsd	%xmm2, %xmm0
	movl	$.LC3, %edi
	movl	$2, %eax
	call	printf
	movl	$0, %edi
	call	pthread_exit
main:
	pushq	%rbp
	movq	%rsp, %rbp
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
	ret
