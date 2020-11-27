#ifdef JEMALLOC

#include <jemalloc/jemalloc.h>

#include "compare.h"

/**
 *  当使用 -ljemalloc 编译后，将使用je_malloc的内存接口
 *  无需更改用户名， 但是如果使用 je_前缀，将显式使用 je_malloc 接口才行
 */


inline void jemalloc_malloc_init(void*user_ptr)
{

}

inline void* jemalloc_malloc(void*user_ptr, size_t size)
{
    return je_malloc(size);
}
inline void jemalloc_test(void*user_ptr, void*ptr, size_t size)
{
    malloc_common_test(ptr, size);
}
inline void jemalloc_free(void*user_ptr, void*ptr)
{
    je_free(ptr);
}
#endif
