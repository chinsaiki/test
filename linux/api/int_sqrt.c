#include <stdio.h>
#include <stdint.h>
#include <linux/types.h>

#ifndef __always_inline
#define __always_inline
#endif
#define EXPORT_SYMBOL(x)

#ifdef __x86_64__
#define BITS_PER_LONG 64
#else
#define BITS_PER_LONG 32
#endif

/**
 * __fls - find last (most-significant) set bit in a long word
 * @word: the word to search
 *
 * Undefined if no set bit exists, so code should check against 0 first.
 */
static __always_inline unsigned long __fls(unsigned long word)
{
	return (sizeof(word) * 8) - 1 - __builtin_clzl(word);
}

/**
 * int_sqrt - computes the integer square root
 * @x: integer of which to calculate the sqrt
 *
 * Computes: floor(sqrt(x))
 */
unsigned long int_sqrt(unsigned long x)
{
	unsigned long b, m, y = 0;

	if (x <= 1)
		return x;

	m = 1UL << (__fls(x) & ~1UL);
	while (m != 0) {
		b = y + m;
		y >>= 1;

		if (x >= b) {
			x -= b;
			y += m;
		}
		m >>= 2;
	}

	return y;
}
EXPORT_SYMBOL(int_sqrt);

#if BITS_PER_LONG < 64
/**
 * int_sqrt64 - strongly typed int_sqrt function when minimum 64 bit input
 * is expected.
 * @x: 64bit integer of which to calculate the sqrt
 */
u32 int_sqrt64(u64 x)
{
	u64 b, m, y = 0;

	if (x <= ULONG_MAX)
		return int_sqrt((unsigned long) x);

	m = 1ULL << ((fls64(x) - 1) & ~1ULL);
	while (m != 0) {
		b = y + m;
		y >>= 1;

		if (x >= b) {
			x -= b;
			y += m;
		}
		m >>= 2;
	}

	return y;
}
EXPORT_SYMBOL(int_sqrt64);
#endif

int main()
{
	printf("int_sqrt   = %ld\n", int_sqrt(4));
	printf("int_sqrt   = %ld\n", int_sqrt(64));
	printf("int_sqrt   = %ld\n", int_sqrt(256));
	printf("int_sqrt   = %ld\n", int_sqrt(24));
	printf("int_sqrt   = %ld\n", int_sqrt(26));
}

