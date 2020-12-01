#include <stdio.h>

#define OFFSETOF_CURTHREAD (0)
#define __pure2
#define __inline inline 

//段错误
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnull-dereference"
#endif
static __inline __pure2 void*
__curthread(void)
{
	struct thread *td;

	__asm("movq %%gs:%1,%0" : "=r" (td)
	    : "m" (*(char *)OFFSETOF_CURTHREAD));
	return (td);
}
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#define	OFFSETOF_CURPCB		32
static __inline __pure2 struct pcb *
__curpcb(void)
{
	struct pcb *pcb;

	__asm("movq %%gs:%1,%0" : "=r" (pcb) : "m" (*(char *)OFFSETOF_CURPCB));
	return (pcb);
}

int main()
{
    void *p = NULL;

    p = __curpcb();

    printf("thread %p\n", p);
}
