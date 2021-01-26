//pq.h	include\linux\raid	6354	2020/12/3	8
//(jiffies: 时间量；耗尽后会发生什么)

/* Some definitions to allow code to be compiled for testing in userspace */
#ifndef __KERNEL__

# define jiffies	raid6_jiffies()
# define printk 	printf
# define pr_err(format, ...) fprintf(stderr, format, ## __VA_ARGS__)
# define pr_info(format, ...) fprintf(stdout, format, ## __VA_ARGS__)
# define GFP_KERNEL	0
# define __get_free_pages(x, y)	((unsigned long)mmap(NULL, PAGE_SIZE << (y), \
						     PROT_READ|PROT_WRITE,   \
						     MAP_PRIVATE|MAP_ANONYMOUS,\
						     0, 0))
# define free_pages(x, y)	munmap((void *)(x), PAGE_SIZE << (y))

static inline void cpu_relax(void)
{
	/* Nothing */
}

#undef  HZ
#define HZ 1000
static inline uint32_t raid6_jiffies(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec*1000 + tv.tv_usec/1000;
}

#endif /* ! __KERNEL__ */

