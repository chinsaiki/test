#include <stdio.h>

unsigned long timer_get_ticks(void)
{
    unsigned long ret;
    union
    {
        unsigned long tsc_64;
        struct
        {
            unsigned int lo_32;
            unsigned int hi_32;
        };
    } tsc;

    __asm volatile("rdtsc" :
             "=a" (tsc.lo_32),
             "=d" (tsc.hi_32));

     ret = ((unsigned long)tsc.tsc_64);
     return ret;
}

int main()
{
    unsigned long start = 0;
	while(1) {
        start = timer_get_ticks();
		printf("%ld.\n", timer_get_ticks()-start);
		sleep(1);
	}
}

