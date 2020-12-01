#ifdef LIBVMEM

#include <libvmem.h>
#include <string.h>
#include "compare.h"

VMEM *vmem;

inline void vmem_malloc_init(void*user_ptr)
{
    int ret;
    char *buffer;
    posix_memalign(&buffer, getpagesize(), VMEM_MIN_POOL*2);
    memset(buffer, 0, VMEM_MIN_POOL*2);

    vmem = vmem_create_in_region(buffer, VMEM_MIN_POOL*2);
}

inline void* vmem_malloc_malloc(void*user_ptr, size_t size)
{
    return vmem_malloc(vmem, size);
}
inline void vmem_malloc_test(void*user_ptr, void*ptr, size_t size)
{
    malloc_common_test(ptr, size);
}
inline void vmem_malloc_free(void*user_ptr, void*ptr)
{
    vmem_free(vmem, ptr);
}
#endif
