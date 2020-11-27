#ifdef TCMALLOC
#include <gperftools/tcmalloc.h>

#include "compare.h"

inline void tcmalloc_malloc_init(void*user_ptr)
{
    
}

inline void* tcmalloc_malloc(void *user_ptr, size_t size)
{
    return tc_malloc(size);
}
inline void tcmalloc_test(void *user_ptr, void*ptr, size_t size)
{
    malloc_common_test(ptr, size);
}
inline void tcmalloc_free(void *user_ptr, void*ptr)
{
    tc_free(ptr);
}

#endif
