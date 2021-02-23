
//irqflags.h	arch\x86\include\asm	4357	2020/4/17	60
static inline void native_irq_disable(void)
{
	asm volatile("cli": : :"memory");
}

static inline void native_irq_enable(void)
{
	asm volatile("sti": : :"memory");
}

//irqflags.h	arch\x86\include\asm	4357	2020/4/17	60
static inline notrace void arch_local_irq_disable(void)
{
	native_irq_disable();
}

static inline notrace void arch_local_irq_enable(void)
{
	native_irq_enable();
}

