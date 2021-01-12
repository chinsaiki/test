#pragma once

#include <stdio.h>
#include <malloc.h>
#include <assert.h>

#define RTE_CACHE_LINE_SIZE 64
#define RTE_MEMZONE_NAMESIZE 32

/** Cache line mask. */
#define RTE_CACHE_LINE_MASK (RTE_CACHE_LINE_SIZE-1)

#ifndef offsetof
#define offsetof(type, member) __builtin_offsetof(type, member)
#endif

/* Required compiler attributes */
#define likely(x)	__builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)

/**
 * Compiler barrier.
 *
 * Guarantees that operation reordering does not occur at compile time
 * for operations directly before and after the barrier.
 */
#define	rte_compiler_barrier() do {		\
	asm volatile ("" : : : "memory");	\
} while(0)
    
#define	rte_mb() _mm_mfence()
    
#define	rte_wmb() _mm_sfence()
    
#define	rte_rmb() _mm_lfence()
    
#define rte_smp_wmb() rte_compiler_barrier()
    
#define rte_smp_rmb() rte_compiler_barrier()

#include <emmintrin.h>
static inline void rte_pause(void)
{
	_mm_pause();
}
#define RTE_MAX_LCORE 4
#if RTE_MAX_LCORE == 1
#define MPLOCKED                        /**< No need to insert MP lock prefix. */
#else
#define MPLOCKED        "lock ; "       /**< Insert MP lock prefix. */
#endif

static inline int
rte_atomic32_cmpset(volatile uint32_t *dst, uint32_t exp, uint32_t src)
{
	uint8_t res;

	asm volatile(
			MPLOCKED
			"cmpxchgl %[src], %[dst];"
			"sete %[res];"
			: [res] "=a" (res),     /* output */
			  [dst] "=m" (*dst)
			: [src] "r" (src),      /* input */
			  "a" (exp),
			  "m" (*dst)
			: "memory");            /* no-clobber list */
	return res;
}


#define RTE_ASSERT assert


/**
 * Force alignment
 */
#define __rte_aligned(a) __attribute__((__aligned__(a)))

#ifdef RTE_ARCH_STRICT_ALIGN
typedef uint64_t unaligned_uint64_t __rte_aligned(1);
typedef uint32_t unaligned_uint32_t __rte_aligned(1);
typedef uint16_t unaligned_uint16_t __rte_aligned(1);
#else
typedef uint64_t unaligned_uint64_t;
typedef uint32_t unaligned_uint32_t;
typedef uint16_t unaligned_uint16_t;
#endif

/** Force alignment to cache line. */
#define __rte_cache_aligned __rte_aligned(RTE_CACHE_LINE_SIZE)


/**
 * Macro to align a pointer to a given power-of-two. The resultant
 * pointer will be a pointer of the same type as the first parameter, and
 * point to an address no higher than the first parameter. Second parameter
 * must be a power-of-two value.
 */
#define RTE_PTR_ALIGN_FLOOR(ptr, align) \
	((typeof(ptr))RTE_ALIGN_FLOOR((uintptr_t)ptr, align))

/**
 * Macro to align a value to a given power-of-two. The resultant value
 * will be of the same type as the first parameter, and will be no
 * bigger than the first parameter. Second parameter must be a
 * power-of-two value.
 */
#define RTE_ALIGN_FLOOR(val, align) \
	(typeof(val))((val) & (~((typeof(val))((align) - 1))))

/**
 * Macro to align a pointer to a given power-of-two. The resultant
 * pointer will be a pointer of the same type as the first parameter, and
 * point to an address no lower than the first parameter. Second parameter
 * must be a power-of-two value.
 */
#define RTE_PTR_ALIGN_CEIL(ptr, align) \
	RTE_PTR_ALIGN_FLOOR((typeof(ptr))RTE_PTR_ADD(ptr, (align) - 1), align)

/**
 * Macro to align a value to a given power-of-two. The resultant value
 * will be of the same type as the first parameter, and will be no lower
 * than the first parameter. Second parameter must be a power-of-two
 * value.
 */
#define RTE_ALIGN_CEIL(val, align) \
	RTE_ALIGN_FLOOR(((val) + ((typeof(val)) (align) - 1)), align)

/**
 * Macro to align a pointer to a given power-of-two. The resultant
 * pointer will be a pointer of the same type as the first parameter, and
 * point to an address no lower than the first parameter. Second parameter
 * must be a power-of-two value.
 * This function is the same as RTE_PTR_ALIGN_CEIL
 */
#define RTE_PTR_ALIGN(ptr, align) RTE_PTR_ALIGN_CEIL(ptr, align)

/**
 * Macro to align a value to a given power-of-two. The resultant
 * value will be of the same type as the first parameter, and
 * will be no lower than the first parameter. Second parameter
 * must be a power-of-two value.
 * This function is the same as RTE_ALIGN_CEIL
 */
#define RTE_ALIGN(val, align) RTE_ALIGN_CEIL(val, align)

/**
 * Macro to align a value to the multiple of given value. The resultant
 * value will be of the same type as the first parameter and will be no lower
 * than the first parameter.
 */
#define RTE_ALIGN_MUL_CEIL(v, mul) \
	(((v + (typeof(v))(mul) - 1) / ((typeof(v))(mul))) * (typeof(v))(mul))

/**
 * Macro to align a value to the multiple of given value. The resultant
 * value will be of the same type as the first parameter and will be no higher
 * than the first parameter.
 */
#define RTE_ALIGN_MUL_FLOOR(v, mul) \
	((v / ((typeof(v))(mul))) * (typeof(v))(mul))

/**
 * Macro to align value to the nearest multiple of the given value.
 * The resultant value might be greater than or less than the first parameter
 * whichever difference is the lowest.
 */
#define RTE_ALIGN_MUL_NEAR(v, mul)				\
	({							\
		typeof(v) ceil = RTE_ALIGN_MUL_CEIL(v, mul);	\
		typeof(v) floor = RTE_ALIGN_MUL_FLOOR(v, mul);	\
		(ceil - v) > (v - floor) ? floor : ceil;	\
	})

/**
 * Checks if a pointer is aligned to a given power-of-two value
 *
 * @param ptr
 *   The pointer whose alignment is to be checked
 * @param align
 *   The power-of-two value to which the ptr should be aligned
 *
 * @return
 *   True(1) where the pointer is correctly aligned, false(0) otherwise
 */
//static inline int
//rte_is_aligned(void *ptr, unsigned align)
//{
//	return RTE_PTR_ALIGN(ptr, align) == ptr;
//}


/**
 * Hint never returning function
 */
#define __rte_noreturn __attribute__((noreturn))

/**
 * Force a function to be inlined
 */
#define __rte_always_inline inline __attribute__((always_inline))

/**
 * Force a function to be noinlined
 */
#define __rte_noinline __attribute__((noinline))

/**
 * Hint function in the hot path
 */
#define __rte_hot __attribute__((hot))

/**
 * Hint function in the cold path
 */
#define __rte_cold __attribute__((cold))

/** C extension macro for environments lacking C11 features. */
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 201112L
#define RTE_STD_C11 __extension__
#else
#define RTE_STD_C11
#endif
/**
 * definition to mark a variable or function parameter as used so
 * as to avoid a compiler warning
 */
#define RTE_SET_USED(x) (void)(x)

/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2015 Neil Horman <nhorman@tuxdriver.com>.
 * All rights reserved.
 */

#ifndef _RTE_COMPAT_H_
#define _RTE_COMPAT_H_

#ifndef ALLOW_EXPERIMENTAL_API

#define __rte_experimental \
__attribute__((deprecated("Symbol is not yet part of stable ABI"), \
section(".text.experimental")))

#else

#define __rte_experimental \
__attribute__((section(".text.experimental")))

#endif

#ifndef ALLOW_INTERNAL_API

#define __rte_internal \
__attribute__((error("Symbol is not public ABI"), \
section(".text.internal")))

#else

#define __rte_internal \
__attribute__((section(".text.internal")))

#endif

#endif /* _RTE_COMPAT_H_ */

/**
 * 128-bit integer structure.
 */
RTE_STD_C11
typedef struct {
	RTE_STD_C11
	union {
		uint64_t val[2];
#ifdef RTE_ARCH_64
		__extension__ __int128 int128;
#endif
	};
} __rte_aligned(16) rte_int128_t;



#define RTE_LOG(MOD, LEVEL, fmt...) printf(fmt)

#define rte_panic(fmt...) do{printf(fmt);assert(0);}while(0)

#define rte_zmalloc(name, align, size) memalign(align, size)
//#define rte_malloc rte_zmalloc
static void*
rte_malloc(size_t size)
{
	return malloc(size);
}
static void
rte_free(void *addr)
{
	return free(addr);
}

#define strlcpy(dst, src, size) rte_strlcpy(dst, src, size)

/**
 * @internal
 * DPDK-specific version of strlcpy for systems without
 * libc or libbsd copies of the function
 */
static inline size_t
rte_strlcpy(char *dst, const char *src, size_t size)
{
	return (size_t)snprintf(dst, size, "%s", src);
}


/**
 * Combines 32b inputs most significant set bits into the least
 * significant bits to construct a value with the same MSBs as x
 * but all 1's under it.
 *
 * @param x
 *    The integer whose MSBs need to be combined with its LSBs
 * @return
 *    The combined value.
 */
static inline uint32_t
rte_combine32ms1b(uint32_t x)
{
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;

	return x;
}


/**
 * Aligns input parameter to the next power of 2
 *
 * @param x
 *   The integer value to align
 *
 * @return
 *   Input parameter aligned to the next power of 2
 */
static inline uint32_t
rte_align32pow2(uint32_t x)
{
	x--;
	x = rte_combine32ms1b(x);

	return x + 1;
}

