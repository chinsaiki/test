#include <stdint.h>

#include "bits_find.h"
#include "bitmap.h"
#include "bits_common.h"


/*
 * This is a common helper function for find_next_bit, find_next_zero_bit, and
 * find_next_and_bit. The differences are:
 *  - The "invert" argument, which is XORed with each fetched word before
 *    searching it for one bits.
 *  - The optional "addr2", which is anded with "addr1" if present.
 */
static inline unsigned long __find_next_bit(const unsigned long *addr1,
		const unsigned long *addr2, unsigned long nbits,
		unsigned long start, unsigned long invert)
{
	unsigned long tmp;

	if (unlikely(start >= nbits))
		return nbits;

	tmp = addr1[start / BITS_PER_LONG];
	if (addr2)
		tmp &= addr2[start / BITS_PER_LONG];
	tmp ^= invert;

	/* Handle 1st word. */
	tmp &= BITMAP_FIRST_WORD_MASK(start);
	start = round_down(start, BITS_PER_LONG);

	while (!tmp) {
		start += BITS_PER_LONG;
		if (start >= nbits)
			return nbits;

		tmp = addr1[start / BITS_PER_LONG];
		if (addr2)
			tmp &= addr2[start / BITS_PER_LONG];
		tmp ^= invert;
	}

	return MIN(start + _ctzl(tmp), nbits);
}


/*
 * Find the next set bit in a memory region.
 */
unsigned long find_next_bit(const unsigned long *addr, unsigned long size,
                unsigned long offset)
{
    return __find_next_bit(addr, NULL, size, offset, 0UL);
}


/*
 * Find the first set bit in a memory region.
 */
unsigned long find_first_bit(const unsigned long *addr, unsigned long size)
{
    unsigned long idx;

    for (idx = 0; idx * BITS_PER_LONG < size; idx++) {
        if (addr[idx])
            return MIN(idx * BITS_PER_LONG + _ctzl(addr[idx]), size);
    }

    return size;
}

/*
 * Find the first cleared bit in a memory region.
 */
unsigned long find_first_zero_bit(const unsigned long *addr, unsigned long size)
{
	unsigned long idx;

	for (idx = 0; idx * BITS_PER_LONG < size; idx++) {
		if (addr[idx] != ~0UL)
			return MIN(idx * BITS_PER_LONG + _ffz(addr[idx]), size);
	}

	return size;
}

unsigned long find_next_zero_bit(const unsigned long *addr, unsigned long size,
				 unsigned long offset)
{
	return __find_next_bit(addr, NULL, size, offset, ~0UL);
}


unsigned long find_next_and_bit(const unsigned long *addr1,
     const unsigned long *addr2, unsigned long size,
     unsigned long offset)
{
    return __find_next_bit(addr1, addr2, size, offset, 0UL);
}


unsigned long find_next_clump8(unsigned long *clump, const unsigned long *addr,
                unsigned long size, unsigned long offset)
{
    offset = find_next_bit(addr, size, offset);
    if (offset == size)
        return size;

    offset = round_down(offset, 8);
    *clump = bitmap_get_value8(addr, offset);

    return offset;
}


unsigned long find_last_bit(const unsigned long *addr, unsigned long size)
{
    if (size) {
        unsigned long val = BITMAP_LAST_WORD_MASK(size);
        unsigned long idx = (size-1) / BITS_PER_LONG;

        do {
            val &= addr[idx];
            if (val)
                return idx * BITS_PER_LONG + _fls64(val);

            val = ~0ul;
        } while (idx--);
    }
    return size;
}


