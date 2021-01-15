#pragma once

#include <endian.h>

#ifdef __x86_64__
#define BITS_PER_LONG 64
#else
#error just support x86-64
#endif



#ifndef likely
#define likely(exp) __builtin_expect(!!(exp), 1)
#endif 

#ifndef unlikely
#define unlikely(exp) __builtin_expect(!!(exp), 0)
#endif


#define BIT(nr)			(UL(1) << (nr))/*   <rongtao 2019.10>*/
#define BIT_ULL(nr)		(ULL(1) << (nr))
#define BIT_MASK(nr)		(UL(1) << ((nr) % BITS_PER_LONG))
#define BIT_WORD(nr)		((nr) / BITS_PER_LONG)
#define BIT_ULL_MASK(nr)	(ULL(1) << ((nr) % BITS_PER_LONG_LONG))
#define BIT_ULL_WORD(nr)	((nr) / BITS_PER_LONG_LONG)
#define BITS_PER_BYTE		8


/*
 * Create a contiguous bitmask starting at bit position @l and ending at
 * position @h. For example
 * GENMASK_ULL(39, 21) gives us the 64bit vector 0x000000ffffe00000.
 *//*   <rongtao 2019.10>*/
#define GENMASK(h, l) \
	(((~ UL(0)) - (UL(1) << (l)) + 1) & \
	 (~ UL(0) >> (BITS_PER_LONG - 1 - (h))))


#define GENMASK_ULL(h, l) \
	(((~ ULL(0)) - (ULL(1) << (l)) + 1) & \
	 (~ ULL(0) >> (BITS_PER_LONG_LONG - 1 - (h))))



/**
 * DOC: bitmap bitops
 *
 *
 *  set_bit(bit, addr)                  *addr |= bit
 *  clear_bit(bit, addr)                *addr &= ~bit
 *  change_bit(bit, addr)               *addr ^= bit
 *  test_bit(bit, addr)                 Is bit set in *addr?
 *  test_and_set_bit(bit, addr)         Set bit and return old value
 *  test_and_clear_bit(bit, addr)       Clear bit and return old value
 *  test_and_change_bit(bit, addr)      Change bit and return old value
 *  find_first_zero_bit(addr, nbits)    Position first zero bit in *addr
 *  find_first_bit(addr, nbits)         Position first set bit in *addr
 *  find_next_zero_bit(addr, nbits, bit)
 *                                      Position next zero bit in *addr >= bit
 *  find_next_bit(addr, nbits, bit)     Position next set bit in *addr >= bit
 *  find_next_and_bit(addr1, addr2, nbits, bit)
 *                                      Same as find_next_bit, but in
 *                                      (*addr1 & *addr2)
 *
 */

/* How many bits in an unsigned long */
#define ____BITS_PER_LONG (8 * sizeof(unsigned long))

static inline void set_bit(int nr, volatile void *addr)
{
    volatile unsigned long *laddr = addr;
	laddr[nr / ____BITS_PER_LONG] |= 1UL << (nr % ____BITS_PER_LONG);
}

static inline void clear_bit(int nr, volatile void *addr)
{
    volatile unsigned long *laddr = addr;
	laddr[nr / ____BITS_PER_LONG] &= ~(1UL << (nr % ____BITS_PER_LONG));
}

static inline void change_bit(int nr, volatile void *addr)
{
	int	mask;
	volatile unsigned int *a = addr;
	unsigned long tmp;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);

	__asm__ __volatile__ (
		"1:						\n\t"
		"movli.l	@%1, %0	! change_bit		\n\t"
		"xor		%2, %0				\n\t"
		"movco.l	%0, @%1				\n\t"
		"bf		1b				\n\t"
		: "=&z" (tmp)
		: "r" (a), "r" (mask)
		: "t", "memory"
	);
}

static inline int test_and_set_bit(int nr, volatile void *addr)
{
	int	mask, retval;
	volatile unsigned int *a = addr;
	unsigned long tmp;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);

	__asm__ __volatile__ (
		"1:						\n\t"
		"movli.l	@%2, %0	! test_and_set_bit	\n\t"
		"mov		%0, %1				\n\t"
		"or		%3, %0				\n\t"
		"movco.l	%0, @%2				\n\t"
		"bf		1b				\n\t"
		"and		%3, %1				\n\t"
		: "=&z" (tmp), "=&r" (retval)
		: "r" (a), "r" (mask)
		: "t", "memory"
	);

	return retval != 0;
}

static inline int test_and_clear_bit(int nr, volatile void *addr)
{
	int	mask, retval;
	volatile unsigned int *a = addr;
	unsigned long tmp;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);

	__asm__ __volatile__ (
		"1:						\n\t"
		"movli.l	@%2, %0	! test_and_clear_bit	\n\t"
		"mov		%0, %1				\n\t"
		"and		%4, %0				\n\t"
		"movco.l	%0, @%2				\n\t"
		"bf		1b				\n\t"
		"and		%3, %1				\n\t"
		"synco						\n\t"
		: "=&z" (tmp), "=&r" (retval)
		: "r" (a), "r" (mask), "r" (~mask)
		: "t", "memory"
	);

	return retval != 0;
}

static inline int test_and_change_bit(int nr, volatile void *addr)
{
	int	mask, retval;
	volatile unsigned int *a = addr;
	unsigned long tmp;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);

	__asm__ __volatile__ (
		"1:						\n\t"
		"movli.l	@%2, %0	! test_and_change_bit	\n\t"
		"mov		%0, %1				\n\t"
		"xor		%3, %0				\n\t"
		"movco.l	%0, @%2				\n\t"
		"bf		1b				\n\t"
		"and		%3, %1				\n\t"
		"synco						\n\t"
		: "=&z" (tmp), "=&r" (retval)
		: "r" (a), "r" (mask)
		: "t", "memory"
	);

	return retval != 0;
}


/**
 * __set_bit - Set a bit in memory
 * @nr: the bit to set
 * @addr: the address to start counting from
 *
 * Unlike set_bit(), this function is non-atomic and may be reordered.
 * If it's called on the same region of memory simultaneously, the effect
 * may be that only one operation succeeds.
 */
static inline void __set_bit(int nr, volatile unsigned long *addr)
{
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);

	*p  |= mask;
}

static inline void __clear_bit(int nr, volatile unsigned long *addr)
{
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);

	*p &= ~mask;
}

/**
 * __change_bit - Toggle a bit in memory
 * @nr: the bit to change
 * @addr: the address to start counting from
 *
 * Unlike change_bit(), this function is non-atomic and may be reordered.
 * If it's called on the same region of memory simultaneously, the effect
 * may be that only one operation succeeds.
 */
static inline void __change_bit(int nr, volatile unsigned long *addr)
{
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);

	*p ^= mask;
}

/**
 * __test_and_set_bit - Set a bit and return its old value
 * @nr: Bit to set
 * @addr: Address to count from
 *
 * This operation is non-atomic and can be reordered.
 * If two examples of this operation race, one can appear to succeed
 * but actually fail.  You must protect multiple accesses with a lock.
 */
static inline int __test_and_set_bit(int nr, volatile unsigned long *addr)
{
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);
	unsigned long old = *p;

	*p = old | mask;
	return (old & mask) != 0;
}

/**
 * __test_and_clear_bit - Clear a bit and return its old value
 * @nr: Bit to clear
 * @addr: Address to count from
 *
 * This operation is non-atomic and can be reordered.
 * If two examples of this operation race, one can appear to succeed
 * but actually fail.  You must protect multiple accesses with a lock.
 */
static inline int __test_and_clear_bit(int nr, volatile unsigned long *addr)
{
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);
	unsigned long old = *p;

	*p = old & ~mask;
	return (old & mask) != 0;
}

/* WARNING: non atomic and it can be reordered! */
static inline int __test_and_change_bit(int nr,
					    volatile unsigned long *addr)
{
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);
	unsigned long old = *p;

	*p = old ^ mask;
	return (old & mask) != 0;
}

/**
 * test_bit - Determine whether a bit is set
 * @nr: bit number to test
 * @addr: Address to start counting from
 */
static inline int test_bit(int nr, const volatile unsigned long *addr)
{
	return 1UL & (addr[BIT_WORD(nr)] >> (nr & (BITS_PER_LONG-1)));
}



/**
 * hex_to_bin - convert a hex digit to its real value
 * @ch: ascii character represents hex digit
 *
 * hex_to_bin() converts one hex digit to its actual value or -1 in case of bad
 * input.
 */
static int hex_to_bin(char ch)
{
	if ((ch >= '0') && (ch <= '9'))
		return ch - '0';
	ch = tolower(ch);
	if ((ch >= 'a') && (ch <= 'f'))
		return ch - 'a' + 10;
	return -1;
}


#ifndef ROUND_UP
#define ROUND_UP(a, b) ((a) % (b) ? ((a) + (b)) - ((a) % (b)) : (a))
#endif


#ifndef DIV_ROUND_UP
#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))
#endif


/*
 * This looks more complex than it should be. But we need to
 * get the type for the ~ right in round_down (it needs to be
 * as wide as the result!), and we want to evaluate the macro
 * arguments just once each.
 */
#ifndef __round_mask
#define __round_mask(x, y) ((__typeof__(x))((y)-1))
#endif

/**
 * round_up - round up to next specified power of 2
 * @x: the value to round
 * @y: multiple to round up to (must be a power of 2)
 *
 * Rounds @x up to next multiple of @y (which must be a power of 2).
 * To perform arbitrary rounding up, use roundup() below.
 */
#ifndef round_up
#define round_up(x, y) ((((x)-1) | __round_mask(x, y))+1)
#endif

/**
 * round_down - round down to next specified power of 2
 * @x: the value to round
 * @y: multiple to round down to (must be a power of 2)
 *
 * Rounds @x down to next multiple of @y (which must be a power of 2).
 * To perform arbitrary rounding down, use rounddown() below.
 */
#ifndef round_down
#define round_down(x, y) ((x) & ~__round_mask(x, y))
#endif 

#ifndef roundup
#define roundup(x, y) (                                \
{                                                      \
	const typeof(y) __y = y;		       \
	(((x) + (__y - 1)) / __y) * __y;	       \
}                                                      \
)
#endif

/*
 * 'kernel.h' contains some often-used function prototypes etc
 */
#define __ALIGN_KERNEL(x, a)		__ALIGN_KERNEL_MASK(x, (typeof(x))(a) - 1)
#define __ALIGN_KERNEL_MASK(x, mask)	(((x) + (mask)) & ~(mask))

#define __KERNEL_DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))


/* @a is a power of 2 value */
#define ALIGN(x, a)		__ALIGN_KERNEL((x), (a))
#define ALIGN_DOWN(x, a)	__ALIGN_KERNEL((x) - ((a) - 1), (a))
#define __ALIGN_MASK(x, mask)	__ALIGN_KERNEL_MASK((x), (mask))
#define PTR_ALIGN(p, a)		((typeof(p))ALIGN((unsigned long)(p), (a)))
#define IS_ALIGNED(x, a)		(((x) & ((typeof(x))(a) - 1)) == 0)


/*
 * Compile time versions of __arch_hweightN()
 */
#define __const_hweight8(w)		\
	((unsigned int)			\
	 ((!!((w) & (1ULL << 0))) +	\
	  (!!((w) & (1ULL << 1))) +	\
	  (!!((w) & (1ULL << 2))) +	\
	  (!!((w) & (1ULL << 3))) +	\
	  (!!((w) & (1ULL << 4))) +	\
	  (!!((w) & (1ULL << 5))) +	\
	  (!!((w) & (1ULL << 6))) +	\
	  (!!((w) & (1ULL << 7)))))

#define __const_hweight16(w) (__const_hweight8(w)  + __const_hweight8((w)  >> 8 ))
#define __const_hweight32(w) (__const_hweight16(w) + __const_hweight16((w) >> 16))
#define __const_hweight64(w) (__const_hweight32(w) + __const_hweight32((w) >> 32))

static inline unsigned int __arch_hweight32(unsigned int w)
{
	return __builtin_popcount(w);
}

static inline unsigned int __arch_hweight16(unsigned int w)
{
	return __builtin_popcount(w & 0xffff);
}

static inline unsigned int __arch_hweight8(unsigned int w)
{
	return __builtin_popcount(w & 0xff);
}

static inline unsigned long __arch_hweight64(uint64_t w)
{
	return __builtin_popcountll(w);
}

/*
 * Generic interface.
 */
#define hweight8(w)  (__builtin_constant_p(w) ? __const_hweight8(w)  : __arch_hweight8(w))
#define hweight16(w) (__builtin_constant_p(w) ? __const_hweight16(w) : __arch_hweight16(w))
#define hweight32(w) (__builtin_constant_p(w) ? __const_hweight32(w) : __arch_hweight32(w))
#define hweight64(w) (__builtin_constant_p(w) ? __const_hweight64(w) : __arch_hweight64(w))


extern unsigned int __sw_hweight8(unsigned int w);
extern unsigned int __sw_hweight16(unsigned int w);
extern unsigned int __sw_hweight32(unsigned int w);
extern unsigned long __sw_hweight64(uint64_t w);

static inline unsigned long hweight_long(unsigned long w)
{
	return sizeof(w) == 4 ? hweight32(w) : hweight64(w);
}


/**
 * CRTL print
 * @param fmt: format, like printf(fmt, ...)'s format
 * @return charector number of print
 */
#define print_emerg(fmt...)    fprintf(stderr, fmt)
#define print_alert(fmt...)    fprintf(stderr, fmt)
#define print_crit(fmt...)     fprintf(stderr, fmt)
#define print_err(fmt...)      fprintf(stderr, fmt)
#define print_warning(fmt...)  fprintf(stdout, fmt)
#define print_notice(fmt...)   fprintf(stdout, fmt)
#define print_info(fmt...)     fprintf(stdout, fmt)
#define print_debug(fmt...)    fprintf(stdout, fmt)
/**
 * CRTL debug switch
 */
#define DEBUG 1
#if DEBUG
 #define __dbg(fmt...) fprintf(stdout, fmt)
#else
 #define __dbg(fmt...)
#endif

#define tolower(c) tolower(c)

#define toupper(c) toupper(c) 

/**
 * some useful amcor
 */
#include <sys/cdefs.h>


#ifdef __GNUC__
#define _packed             __attribute__((packed))
#define _noreturn           __attribute__((noreturn))
/* 'cold' attribute is for optimization, telling the computer this code
 * path is unlikely. */
#define _noreturn_cold      __attribute__ ((noreturn, cold))
#define _unused             __attribute__((unused))
#define _used               __attribute__((used))/*__attribute_used__*/
#define _const              __attribute__((const))
#define _api                __attribute__((visibility("default")))
#define _hidden             __attribute__((visibility("hidden")))
#define _aligned(x)         __attribute__((aligned(x)))
#define _format(func, a, b) __attribute__((__format__ (func, a, b)))
#define _bitwise            __attribute__((bitwise))
#define _inline inline      __attribute__((always_inline)) /*__always_inline*/
#define _nonnull            __nonnull
#define _alias(symbol)      __attribute__((__alias__(#symbol)))
//#define _aligned(x)         __attribute__((__aligned__(x)))
#define _aligned_largest    __attribute__((__aligned__))
#define _naked			    __attribute__((__naked__))
#define _weak               __attribute__((weak))//变量加上weak时，是弱符号。函数加上weak时，是弱引用，可以不实现。
#define _pure		        __attribute__((pure))

#else

#define _packed             
#define _noreturn  
#define _noreturn_cold
#define _unused   
#define _used
#define _const              
#define _api                
#define _hidden             
#define _aligned(x)         
#define _format(func, a, b) 
#define _bitwise 
#define _inline
#define _nonnull            
#define _alias(symbol)
//#define _aligned(x)
#define _aligned_largest
#define _naked	
#define _weak
#define _pure

#endif

/**
 *  x=8: 0000 0000 0000 1000; location of suffix "1" . 
 *  1000 -> 4 
 */
#ifndef _ffs
#define _ffs(integer) __builtin_ffs(integer)
#endif

#ifndef __ffs
#define __ffs(word) __builtin_ctzl(word)
#endif


/**
 * _ctzl - find first bit in word.
 * @word: The word to search
 *
 * Undefined if no bit exists, so code should check against 0 first.
 */
#ifndef _ctzl
#define _ctzl(long_word) __builtin_ctzl(long_word)
#endif



/**
 * _ctzll - find first bit in word.
 * @word: The word to search
 *
 * Undefined if no bit exists, so code should check against 0 first.
 */
#ifndef _ctzll
#define _ctzll(long_long_word) __builtin_ctzll(long_long_word)
#endif




/*
 * _ffz - find first zero in word.
 * @word: The word to search
 *
 * Undefined if no zero exists, so code should check against ~0UL first.
 */
#ifndef _ffz
#define _ffz(x)  _ctzl(~(x))
#endif

/**
 * _ffs64 - find first set bit in a 64 bit word
 * @word: The 64 bit word
 *
 * On 64 bit arches this is a synomyn for _ctzl
 * The result is not defined if no bits are set, so check that @word
 * is non-zero before calling this.
 */
#ifndef _ffs64
#define _ffs64(word)  _ctzl((unsigned long)word)
#endif

/**
 * _fls32 - find last (most-significant) bit set
 * @x: the word to search
 *
 * This is defined the same way as _ffs.
 * Note _fls32(0) = 0, _fls32(1) = 1, _fls32(0x80000000) = 32.
 */
#ifndef _fls32
#define _fls32(x) (x ? sizeof(int) * 8 - __builtin_clz(x) : 0)
#endif

/**
 * _fls64 - find last (most-significant) set bit in a long word
 * @word: the word to search
 *
 * Undefined if no set bit exists, so code should check against 0 first.
 */
#ifndef _fls64
#define _fls64(word) ((sizeof(long) * 8) - 1 - __builtin_clzl(word))
#endif

/**
 * fls64 - find last set bit in a 64-bit word
 * @x: the word to search
 *
 * This is defined in a similar way as the libc and compiler builtin
 * ffsll, but returns the position of the most significant set bit.
 *
 * fls64(value) returns 0 if value is 0 or the position of the last
 * set bit if value is nonzero. The last (most significant) bit is
 * at position 64.
 */
#ifndef fls64
#define fls64(x) (x?_fls64(x) + 1:0)
#endif

#ifndef fls_long
#define fls_long(l) fls64(l)
#endif

/**
 *  x=8: 0000 0000 0000 1000; number of "1" 
 *  8 -> 1
 */
#ifndef _popcount
#define _popcount(integer) __builtin_popcount(integer)
#endif


/**
 *  x=8: 0000 0000 0000 1000; number of "1" 
 *  8 -> 1
 */
#ifndef _popcountll
#define _popcountll(long_long) __builtin_popcountll(long_long)
#endif




/**
 *  x=8: 0000 0000 0000 1000;  number of low "0" 
 *  8 -> 3
 */
#ifndef _ctz
#define _ctz(x) __builtin_ctz(x)
#endif


/**
 *  x=8: 0000 0000 0000 1000;  number of high "0" 
 *  8 -> 28
 */
#ifndef _clz
#define _clz(x) __builtin_clz(x)
#endif

/**
 *  x=8: 0000 0000 0000 1000;  parity of "1" 
 *  8 -> 0
 *  12 -> 0
 *  13 -> 1
 *  1 -> 1
 */
#ifndef _parity
#define _parity(x) __builtin_parity(x)
#endif


/**
 * fls - find last (most-significant) bit set
 * @x: the word to search
 *
 * This is defined the same way as ffs.
 * Note fls(0) = 0, fls(1) = 1, fls(0x80000000) = 32.
 */
#ifndef fls
#define fls(x) ((unsigned int )x ? sizeof(x) * 8 - __builtin_clz(x) : 0)
#endif


#define ABS(x) ((x) >= 0 ? (x) : -(x))
#define MIN(a,b) ((a) <= (b) ? (a) : (b))
#define MAX(a,b) ((a) >= (b) ? (a) : (b))



/**
 * max number
 * @param x: x
 * @param y: y
 */
#ifndef max
#define max(x, y) ({				\
	typeof(x) _max1 = (x);			\
	typeof(y) _max2 = (y);			\
	(void) (&_max1 == &_max2);		\
	_max1 > _max2 ? _max1 : _max2; })
#endif

/**
 * min number
 * @param x: x
 * @param y: y
 */
#ifndef min
#define min(x, y) ({				\
	typeof(x) _min1 = (x);			\
	typeof(y) _min2 = (y);			\
	(void) (&_min1 == &_min2);		\
	_min1 < _min2 ? _min1 : _min2; })
#endif


/**
 * swab 16 bits byteorder
 * @param x: 16 bits in
 * @return the data after swab
 */
#define _swapbyte16(s16) (((s16>>8)&0xff) | ((s16&0xff)<<8))


/**
 * swab 32 bits byteorder
 * @param x: 32 bits in
 * @return the data after swab
 */
#define _swapbyte32(i32) (((i32>>24)&0xff) | ((i32&0xff)<<24) |\
                          ((i32>>8)&0xff00) | ((i32&0xff00)<<8))
    
/**
 * swab 64 bits byteorder
 * @param x: 64 bits in
 * @return the data after swab
 */
#define _swapbyte64(l64) (((l64>>56)&0xff)       | ((l64&0xff)<<56) |\
		                  ((l64>>40)&0xff00)     | ((l64&0xff00)<<40) |\
		                  ((l64>>24)&0xff0000)   | ((l64&0xff0000)<<24) |\
		                  ((l64>> 8)&0xff000000) | ((l64&0xff000000)<<8))
    
                          
/**
 *  Some other swap macro
 */
#if BYTE_ORDER == LITTLE_ENDIAN
#define le16_to_cpu(val) (val)
#define le32_to_cpu(val) (val)
#define le64_to_cpu(val) (val)
#define be16_to_cpu(val) _swapbyte16(val)
#define be32_to_cpu(val) _swapbyte32(val)
#define be64_to_cpu(val) _swapbyte64(val)
                          
#define cpu_to_le16(val) (val)
#define cpu_to_le32(val) (val)
#define cpu_to_le64(val) (val)
#define cpu_to_be16(val) _swapbyte16(val)
#define cpu_to_be32(val) _swapbyte32(val)
#define cpu_to_be64(val) _swapbyte64(val)
#endif
#if BYTE_ORDER == BIG_ENDIAN
#define le16_to_cpu(val) _swapbyte16(val)
#define le32_to_cpu(val) _swapbyte32(val)
#define le64_to_cpu(val) _swapbyte64(val)
#define be16_to_cpu(val) (val)
#define be32_to_cpu(val) (val)
#define be64_to_cpu(val) (val)
                          
#define cpu_to_le16(val) _swapbyte16(val)
#define cpu_to_le32(val) _swapbyte32(val)
#define cpu_to_le64(val) _swapbyte64(val)
#define cpu_to_be16(val) (val)
#define cpu_to_be32(val) (val)
#define cpu_to_be64(val) (val)
#endif

