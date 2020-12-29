#include <stdio.h>
#include <stdint.h>
#include <linux/types.h>


#define RTE_STD_C11
#define CYC_PER_10MHZ 1E7

//    rdpmc.c tools\perf\arch\x86\tests   3417    2020/12/3   117
static uint64_t rdtsc(void)
{
	unsigned int low, high;

	asm volatile("rdtsc" : "=a" (low), "=d" (high));

	return low | ((uint64_t)high) << 32;
}

#if 0
//turbostat.c	tools\power\x86\turbostat	148068	2020/12/3	3862
static unsigned long long rdtsc(void)
{
	unsigned int low, high;

	asm volatile("rdtsc" : "=a" (low), "=d" (high));

	return low | ((unsigned long long)high) << 32;
}
#endif

int main()
{
	uint64_t tsc_hz_start = rdtsc();

    sleep(1);
	uint64_t tsc_hz_end = rdtsc();
    
	printf("CYC_PER_10MHZ   = %ld\n", CYC_PER_10MHZ);
	printf("CYC_PER_10MHZ   = %x\n", CYC_PER_10MHZ);
	printf("tsc_hz          = %ld\n",tsc_hz_end - tsc_hz_start);
}

