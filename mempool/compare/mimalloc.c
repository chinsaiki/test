#ifdef MIMALLOC

#include <mimalloc.h>

#include "compare.h"

/**
 *  当使用 -lmimalloc 编译后，将使用mi_malloc的内存接口
 *  无需更改用户名， 但是如果使用 mi_前缀，将显式使用 mi_malloc 接口才行
 */


inline void mimalloc_malloc_init(void*user_ptr)
{

}

inline void* mimalloc_malloc(void*user_ptr, size_t size)
{
    return mi_malloc(size);
}
inline void mimalloc_test(void*user_ptr, void*ptr, size_t size)
{
    malloc_common_test(ptr, size);
}
inline void mimalloc_free(void*user_ptr, void*ptr)
{
    mi_free(ptr);
}
#endif

