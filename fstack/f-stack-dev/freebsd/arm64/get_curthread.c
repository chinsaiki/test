#include <stdio.h>


static inline void *
get_curthread(void)
{
	void *ret;

	__asm __volatile("mrc p15, 0, %0, c13, c0, 4" : "=r" (ret));
	return (ret);
}

int main()
{
    void *p = NULL;

    p = get_curthread();

    printf("thread %p\n", p);
}

