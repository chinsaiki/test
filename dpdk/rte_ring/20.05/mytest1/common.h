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

/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 */

#ifndef _RTE_TAILQ_H_
#define _RTE_TAILQ_H_

/**
 * @file
 *  Here defines rte_tailq APIs for only internal use
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/queue.h>
#include <stdio.h>
//#include <rte_debug.h>

/** dummy structure type used by the rte_tailq APIs */
struct rte_tailq_entry {
	TAILQ_ENTRY(rte_tailq_entry) next; /**< Pointer entries for a tailq list */
	void *data; /**< Pointer to the data referenced by this tailq entry */
};
/** dummy */
TAILQ_HEAD(rte_tailq_entry_head, rte_tailq_entry);

#define RTE_TAILQ_NAMESIZE 32

/**
 * The structure defining a tailq header entry for storing
 * in the rte_config structure in shared memory. Each tailq
 * is identified by name.
 * Any library storing a set of objects e.g. rings, mempools, hash-tables,
 * is recommended to use an entry here, so as to make it easy for
 * a multi-process app to find already-created elements in shared memory.
 */
struct rte_tailq_head {
	struct rte_tailq_entry_head tailq_head; /**< NOTE: must be first element */
	char name[RTE_TAILQ_NAMESIZE];
};

struct rte_tailq_elem {
	/**
	 * Reference to head in shared mem, updated at init time by
	 * rte_eal_tailqs_init()
	 */
	struct rte_tailq_head *head;
	TAILQ_ENTRY(rte_tailq_elem) next;
	const char name[RTE_TAILQ_NAMESIZE];
};

/**
 * Return the first tailq entry cast to the right struct.
 */
#define RTE_TAILQ_CAST(tailq_entry, struct_name) \
	(struct struct_name *)&(tailq_entry)->tailq_head

/**
 * Utility macro to make looking up a tailqueue for a particular struct easier.
 *
 * @param name
 *   The name of tailq
 *
 * @param struct_name
 *   The name of the list type we are using. (Generally this is the same as the
 *   first parameter passed to TAILQ_HEAD macro)
 *
 * @return
 *   The return value from rte_eal_tailq_lookup, typecast to the appropriate
 *   structure pointer type.
 *   NULL on error, since the tailq_head is the first
 *   element in the rte_tailq_head structure.
 */
#define RTE_TAILQ_LOOKUP(name, struct_name) \
	RTE_TAILQ_CAST(rte_eal_tailq_lookup(name), struct_name)



/**
 * Dump tail queues to a file.
 *
 * @param f
 *   A pointer to a file for output
 */
void rte_dump_tailq(FILE *f);

/**
 * Lookup for a tail queue.
 *
 * Get a pointer to a tail queue header of a tail
 * queue identified by the name given as an argument.
 * Note: this function is not multi-thread safe, and should only be called from
 * a single thread at a time
 *
 * @param name
 *   The name of the queue.
 * @return
 *   A pointer to the tail queue head structure.
 */
struct rte_tailq_head *rte_eal_tailq_lookup(const char *name);

/**
 * Register a tail queue.
 *
 * Register a tail queue from shared memory.
 * This function is mainly used by EAL_REGISTER_TAILQ macro which is used to
 * register tailq from the different dpdk libraries. Since this macro is a
 * constructor, the function has no access to dpdk shared memory, so the
 * registered tailq can not be used before call to rte_eal_init() which calls
 * rte_eal_tailqs_init().
 *
 * @param t
 *   The tailq element which contains the name of the tailq you want to
 *   create (/retrieve when in secondary process).
 * @return
 *   0 on success or -1 in case of an error.
 */
int rte_eal_tailq_register(struct rte_tailq_elem *t);

#define EAL_REGISTER_TAILQ(t) \
RTE_INIT(tailqinitfn_ ##t) \
{ \
	if (rte_eal_tailq_register(&t) < 0) \
		rte_panic("Cannot initialize tailq: %s\n", t.name); \
}

/* This macro permits both remove and free var within the loop safely.*/
#ifndef TAILQ_FOREACH_SAFE
#define TAILQ_FOREACH_SAFE(var, head, field, tvar)		\
	for ((var) = TAILQ_FIRST((head));			\
	    (var) && ((tvar) = TAILQ_NEXT((var), field), 1);	\
	    (var) = (tvar))
#endif

#ifdef __cplusplus
}
#endif

#endif /* _RTE_TAILQ_H_ */

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

#define rte_zmalloc(name, size, align) memalign(size, align)
