
typedef struct {
	u16	     __softirq_pending;
#if IS_ENABLED(CONFIG_KVM_INTEL)
	u8	     kvm_cpu_l1tf_flush_l1d;
#endif
	unsigned int __nmi_count;	/* arch dependent */
#ifdef CONFIG_X86_LOCAL_APIC
	unsigned int apic_timer_irqs;	/* arch dependent */
	unsigned int irq_spurious_count;
	unsigned int icr_read_retry_count;
#endif
#ifdef CONFIG_HAVE_KVM
	unsigned int kvm_posted_intr_ipis;
	unsigned int kvm_posted_intr_wakeup_ipis;
	unsigned int kvm_posted_intr_nested_ipis;
#endif
	unsigned int x86_platform_ipis;	/* arch dependent */
	unsigned int apic_perf_irqs;
	unsigned int apic_irq_work_irqs;
#ifdef CONFIG_SMP
	unsigned int irq_resched_count;
	unsigned int irq_call_count;
#endif
	unsigned int irq_tlb_count;
#ifdef CONFIG_X86_THERMAL_VECTOR
	unsigned int irq_thermal_count;
#endif
#ifdef CONFIG_X86_MCE_THRESHOLD
	unsigned int irq_threshold_count;
#endif
#ifdef CONFIG_X86_MCE_AMD
	unsigned int irq_deferred_error_count;
#endif
#if IS_ENABLED(CONFIG_HYPERV) || defined(CONFIG_XEN)
	unsigned int irq_hv_callback_count;
#endif
#if IS_ENABLED(CONFIG_HYPERV)
	unsigned int irq_hv_reenlightenment_count;
	unsigned int hyperv_stimer0_count;
#endif
} ____cacheline_aligned irq_cpustat_t;

