#include <stdio.h>
#include <stdint.h>
#include <linux/types.h>


#define RTE_STD_C11
#define CYC_PER_10MHZ 1E7

//processor.h	tools\testing\selftests\kvm\include\x86_64	34665	2020/12/3	168

static inline uint64_t rdtsc(void)
{
	uint32_t eax, edx;

	/*
	 * The lfence is to wait (on Intel CPUs) until all previous
	 * instructions have been executed.
	 */
	__asm__ __volatile__("lfence; rdtsc" : "=a"(eax), "=d"(edx));
	return ((uint64_t)edx) << 32 | eax;
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




