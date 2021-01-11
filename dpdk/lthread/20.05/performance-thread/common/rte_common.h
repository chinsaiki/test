#pragma once

#include <assert.h>
#include <malloc.h>
#include <stdint.h>

/**
 * Force a function to be inlined
 */
#ifndef __rte_always_inline
#define __rte_always_inline inline __attribute__((always_inline))
#endif
/**
 * Force alignment
 */
#ifndef __rte_aligned
#define __rte_aligned(a) __attribute__((__aligned__(a)))
#endif

/** Minimum Cache line size. */
#define RTE_CACHE_LINE_MIN_SIZE 64
#define RTE_CACHE_LINE_SIZE 64

/** Force alignment to cache line. */
#define __rte_cache_aligned __rte_aligned(RTE_CACHE_LINE_SIZE)

/** Force minimum cache line alignment. */
#define __rte_cache_min_aligned __rte_aligned(RTE_CACHE_LINE_MIN_SIZE)
#define RTE_MAX_LCORE 4


/** C extension macro for environments lacking C11 features. */
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 201112L
#define RTE_STD_C11 __extension__
#else
#define RTE_STD_C11
#endif

#ifndef __rte_unused
#define __rte_unused __attribute__((unused))
#endif

#define RTE_ASSERT assert
#ifndef UINT8_MAX
#define UINT8_MAX          (int8_t)(~((int8_t)0))	/* 0xFF               */
#define UINT16_MAX         (uint16_t)(~((uint16_t)0))	/* 0xFFFF             */
#define UINT32_MAX         (uint32_t)(~((uint32_t)0))	/* 0xFFFFFFFF         */
#define UINT64_MAX         (uint64_t)(~((uint64_t)0))	/* 0xFFFFFFFFFFFFFFFF */
#define ASCII_MAX          (0x7F)
#endif

#define LCORE_ID_ANY     UINT32_MAX       /**< Any lcore. */


#define RTE_PRIORITY_LOG 101
#define RTE_PRIORITY_BUS 110
#define RTE_PRIORITY_CLASS 120
#define RTE_PRIORITY_LAST 65535

#define RTE_PRIO(prio) \
	RTE_PRIORITY_ ## prio

/**
 * Run function before main() with high priority.
 *
 * @param func
 *   Constructor function.
 * @param prio
 *   Priority number must be above 100.
 *   Lowest number is the first to run.
 */
#ifndef RTE_INIT_PRIO /* Allow to override from EAL */
#define RTE_INIT_PRIO(func, prio) \
static void __attribute__((constructor(RTE_PRIO(prio)), used)) func(void)
#endif

/**
 * Run function before main() with low priority.
 *
 * The constructor will be run after prioritized constructors.
 *
 * @param func
 *   Constructor function.
 */
#define RTE_INIT(func) \
	RTE_INIT_PRIO(func, LAST)

/**
 * Run after main() with low priority.
 *
 * @param func
 *   Destructor function name.
 * @param prio
 *   Priority number must be above 100.
 *   Lowest number is the last to run.
 */
#ifndef RTE_FINI_PRIO /* Allow to override from EAL */
#define RTE_FINI_PRIO(func, prio) \
static void __attribute__((destructor(RTE_PRIO(prio)), used)) func(void)
#endif


/** Number of elements in the array. */
#define	RTE_DIM(a)	(sizeof (a) / sizeof ((a)[0]))


/**
 * Run after main() with high priority.
 *
 * The destructor will be run *before* prioritized destructors.
 *
 * @param func
 *   Destructor function name.
 */
#define RTE_FINI(func) \
	RTE_FINI_PRIO(func, LAST)


/*
 * Allocate memory on specified heap.
 */
static void *
rte_malloc_socket(const char *type, size_t size, unsigned int align,
		int socket_arg)
{
	return memalign(size, align);
}
/*
 * Allocate zero'd memory on specified heap.
 */
static void *
rte_calloc_socket(const char *type, size_t num, size_t size, unsigned align, int socket)
{
	return memalign(num * size, align);
}
/*
 * Allocate zero'd memory on specified heap.
 */
static void *
rte_zmalloc_socket(const char *type, size_t size, unsigned align, int socket)
{
	void *ptr = rte_malloc_socket(type, size, align, socket);
	return ptr;
}

static void
rte_free(void *addr)
{
	return free(addr);
}


static unsigned rte_socket_id(void)
{
    return 0;
}

static inline unsigned
rte_lcore_id(void)
{
	return 0;
}

static int rte_lcore_index(int lcore_id)
{
//	if (unlikely(lcore_id >= RTE_MAX_LCORE))
//		return -1;
//
//	if (lcore_id < 0)
//		lcore_id = (int)rte_lcore_id();

	return 0;
}
//
//static inline uint64_t
//rte_rdtsc(void)
//{
//	union {
//		uint64_t tsc_64;
//		RTE_STD_C11
//		struct {
//			uint32_t lo_32;
//			uint32_t hi_32;
//		};
//	} tsc;
//
//	asm volatile("rdtsc" :
//		     "=a" (tsc.lo_32),
//		     "=d" (tsc.hi_32));
//	return tsc.tsc_64;
//}
//
//static inline uint64_t
//rte_rdtsc_precise(void)
//{
//	rte_mb();
//	return rte_rdtsc();
//}
//
//static inline uint64_t
//rte_get_tsc_cycles(void) { return rte_rdtsc(); }



/**
 * Searches the input parameter for the least significant set bit
 * (starting from zero).
 * If a least significant 1 bit is found, its bit index is returned.
 * If the content of the input parameter is zero, then the content of the return
 * value is undefined.
 * @param v
 *     input parameter, should not be zero.
 * @return
 *     least significant set bit in the input parameter.
 */
static inline uint32_t
rte_bsf32(uint32_t v)
{
	return (uint32_t)__builtin_ctz(v);
}


static uint64_t
rte_rand(void)
{
//	struct rte_rand_state *state;
//
//	state = __rte_rand_get_state();
//
//	return __rte_rand_lfsr258(state);
    return ((uint64_t)rand())<<32 | rand();
}
static uint64_t
rte_get_tsc_hz(void)
{
//	return eal_tsc_resolution_hz;
    return 2700000000;
}

