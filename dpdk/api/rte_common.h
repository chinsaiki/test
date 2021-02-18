

#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>


/**
 * Force a function to be inlined
 */
#ifndef __rte_always_inline
#define __rte_always_inline inline __attribute__((always_inline))
#endif

