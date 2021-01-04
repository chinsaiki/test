#include <stdio.h>

struct mm_struct {
    struct {
        unsigned long count[64];
    }rss_stat;
};


/*
 * per-process(per-mm_struct) statistics.
 */
static inline unsigned long get_mm_counter(struct mm_struct *mm, int member)
{
	long val = atomic_long_read(&mm->rss_stat.count[member]);

#ifdef SPLIT_RSS_COUNTING
	/*
	 * counter is updated in asynchronous manner and may go to minus.
	 * But it's never be expected number for users.
	 */
	if (val < 0)
		val = 0;
#endif
	return (unsigned long)val;
}

static inline void add_mm_counter(struct mm_struct *mm, int member, long value)
{
	atomic_long_add(value, &mm->rss_stat.count[member]);
}

static inline void inc_mm_counter(struct mm_struct *mm, int member)
{
	atomic_long_inc(&mm->rss_stat.count[member]);
}

static inline void dec_mm_counter(struct mm_struct *mm, int member)
{
	atomic_long_dec(&mm->rss_stat.count[member]);
}

int main()
{
    struct mm_struct mm;
    inc_mm_counter(&mm, 1);

    printf("%ld\n", mm.rss_stat.count[1]);
    
}
