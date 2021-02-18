#include <stdio.h>
#include <malloc.h>

#define RTE_FORCE_INTRINSICS 1

#ifdef RTE_FORCE_INTRINSICS
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)
#define rte_bswap16(x) __builtin_bswap16(x)
#endif

#define rte_bswap32(x) __builtin_bswap32(x)

#define rte_bswap64(x) __builtin_bswap64(x)

#endif

int main()
{
	unsigned short us = 0x1234;
	unsigned int ui = 0x12345678;
	unsigned long ul = 0x1234567801020304;

	printf("rte_bswap16(%x) = %x\n", us, rte_bswap16(us));
	printf("rte_bswap32(%x) = %x\n", ui, rte_bswap32(ui));
	printf("rte_bswap64(%lx) = %lx\n", ul, rte_bswap64(ul));
}
