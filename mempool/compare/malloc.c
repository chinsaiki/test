#ifdef PTMALLOC
#include <malloc.h>

#include "compare.h"


inline void sys_glibc_malloc_init(void*user_ptr)
{

}

inline void* sys_glibc_malloc(void *user_ptr, size_t size)
{
    return malloc(size);
}
inline void sys_glibc_test(void *user_ptr, void*ptr, size_t size)
{
    malloc_common_test(ptr, size);
}
inline void sys_glibc_free(void *user_ptr, void*ptr)
{
    free(ptr);
}
#endif
