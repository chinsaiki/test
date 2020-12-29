#include <stdio.h>
#include <stdint.h>
#include <linux/types.h>


#define RTE_STD_C11
#define CYC_PER_10MHZ 1E7

//disable-tsc-ctxt-sw-stress-test.c	tools\testing\selftests\prctl	1782	2020/12/3	50
static uint64_t rdtsc(void)
{
    uint32_t lo, hi;
    /* We cannot use "=A", since this would use %rax on x86_64 */
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return (uint64_t)hi << 32 | lo;
}


int main()
{
	uint64_t tsc_hz_start = rdtsc();

    sleep(1);
	uint64_t tsc_hz_end = rdtsc();
    
	printf("CYC_PER_10MHZ   = %ld\n", CYC_PER_10MHZ);
	printf("CYC_PER_10MHZ   = %x\n", CYC_PER_10MHZ);
	printf("tsc_hz          = %ld\n",tsc_hz_end - tsc_hz_start);
}


