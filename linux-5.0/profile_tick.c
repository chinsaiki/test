//profile.c	kernel	15108	2020/12/3	320

void profile_tick(int type) //为代码监管器采集数据，由中断触发
{
	struct pt_regs *regs = get_irq_regs(); //获取寄存器

	if (!user_mode(regs) && prof_cpu_mask != NULL &&
	    cpumask_test_cpu(smp_processor_id(), prof_cpu_mask))
		profile_hit(type, (void *)profile_pc(regs));
}

