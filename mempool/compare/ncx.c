#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ncx_slab.h"


static size_t  pool_size = 4096000;  //4M 
static u_char  *space;
static ncx_slab_pool_t *sp;


inline void ncx_malloc_init(void *user_ptr)
{
    space = (u_char *)malloc(pool_size);

    sp = (ncx_slab_pool_t*) space;
    sp->addr = space;
    sp->min_shift = 3;
    sp->end = space + pool_size;

    ncx_slab_init(sp);
}

inline void* ncx_malloc_malloc(void *user_ptr, size_t size)
{
    return ncx_slab_alloc(sp, size); 
}
inline void ncx_malloc_test(void *user_ptr, void*ptr, size_t size)
{
    malloc_common_test(ptr, size);
}
inline void ncx_malloc_free(void *user_ptr, void*ptr)
{
    ncx_slab_free(sp, ptr); 
}

