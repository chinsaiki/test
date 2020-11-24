#include <jemalloc/jemalloc.h>

standard_api() {
    void *malloc(size_t size);

    void *calloc(size_t number, size_t size);

    int posix_memalign(void **ptr, size_t alignment, size_t size);

    void *aligned_alloc(size_t alignment, size_t size);

    void *realloc(void *ptr, size_t size);

    void free(void *ptr);

//    当在有前缀条件下安装时，接口为：

    
    void *je_malloc(size_t size);

    void *je_calloc(size_t number, size_t size);

    int je_posix_memalign(void **ptr, size_t alignment, size_t size);

    void *je_aligned_alloc(size_t alignment, size_t size);

    void *je_realloc(void *ptr, size_t size);

    void je_free(void *ptr);
}
