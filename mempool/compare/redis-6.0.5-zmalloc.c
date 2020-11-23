#include "zmalloc.h"

#include "compare.h"


inline void redis_zmalloc_init(void*ptr)
{

}

inline void* redis_zmalloc_malloc(size_t size)
{
    return zmalloc(size);
}
inline void redis_zmalloc_test(void*ptr, size_t size)
{
    malloc_common_test(ptr, size);
}
inline void redis_zmalloc_free(void*ptr)
{
    zfree(ptr);
}


