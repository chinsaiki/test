/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2015 Intel Corporation
 */

#include <stdio.h>
#include <sys/types.h>

#include <stdint.h>
//#include <rte_config.h>
//#include "generic/rte_vect.h"

#if (defined(__ICC) || \
	(defined(_WIN64)) || \
	(__GNUC__ == 4 &&  __GNUC_MINOR__ < 4))

#include <smmintrin.h> /* SSE4 */

#if defined(__AVX__)
#include <immintrin.h>
#endif

#else

#include <x86intrin.h>

#endif

/* Functions to compare multiple of 16 byte keys (up to 128 bytes) */
static int
rte_hash_k16_cmp_eq(const void *key1, const void *key2, size_t key_len)
{
	const __m128i k1 = _mm_loadu_si128((const __m128i *) key1);
	const __m128i k2 = _mm_loadu_si128((const __m128i *) key2);
	const __m128i x = _mm_xor_si128(k1, k2);

	return !_mm_test_all_zeros(x, x);
//    return 1;
}

static int
rte_hash_k32_cmp_eq(const void *key1, const void *key2, size_t key_len)
{
	return rte_hash_k16_cmp_eq(key1, key2, key_len) ||
		rte_hash_k16_cmp_eq((const char *) key1 + 16,
				(const char *) key2 + 16, key_len);
}

static int
rte_hash_k48_cmp_eq(const void *key1, const void *key2, size_t key_len)
{
	return rte_hash_k16_cmp_eq(key1, key2, key_len) ||
		rte_hash_k16_cmp_eq((const char *) key1 + 16,
				(const char *) key2 + 16, key_len) ||
		rte_hash_k16_cmp_eq((const char *) key1 + 32,
				(const char *) key2 + 32, key_len);
}

static int
rte_hash_k64_cmp_eq(const void *key1, const void *key2, size_t key_len)
{
	return rte_hash_k32_cmp_eq(key1, key2, key_len) ||
		rte_hash_k32_cmp_eq((const char *) key1 + 32,
				(const char *) key2 + 32, key_len);
}

static int
rte_hash_k80_cmp_eq(const void *key1, const void *key2, size_t key_len)
{
	return rte_hash_k64_cmp_eq(key1, key2, key_len) ||
		rte_hash_k16_cmp_eq((const char *) key1 + 64,
				(const char *) key2 + 64, key_len);
}

static int
rte_hash_k96_cmp_eq(const void *key1, const void *key2, size_t key_len)
{
	return rte_hash_k64_cmp_eq(key1, key2, key_len) ||
		rte_hash_k32_cmp_eq((const char *) key1 + 64,
				(const char *) key2 + 64, key_len);
}

static int
rte_hash_k112_cmp_eq(const void *key1, const void *key2, size_t key_len)
{
	return rte_hash_k64_cmp_eq(key1, key2, key_len) ||
		rte_hash_k32_cmp_eq((const char *) key1 + 64,
				(const char *) key2 + 64, key_len) ||
		rte_hash_k16_cmp_eq((const char *) key1 + 96,
				(const char *) key2 + 96, key_len);
}

static int
rte_hash_k128_cmp_eq(const void *key1, const void *key2, size_t key_len)
{
	return rte_hash_k64_cmp_eq(key1, key2, key_len) ||
		rte_hash_k64_cmp_eq((const char *) key1 + 64,
				(const char *) key2 + 64, key_len);
}

int main()
{
    uint16_t k1, k2;
    k1 = 1;
    k2 = 2;
    
    rte_hash_k16_cmp_eq(&k1, &k2, 2);
}
