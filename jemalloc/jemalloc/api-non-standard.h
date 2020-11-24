#include <jemalloc/jemalloc.h>

non_standard_api() {

    void *mallocx(size_t size, int flags);

    void *rallocx(void *ptr, size_t size, int flags);

    size_t xallocx(void *ptr, size_t size, size_t extra, int flags);

    size_t sallocx(void *ptr, int flags);

    void dallocx(void *ptr, int flags);

    void sdallocx(void *ptr, size_t size, int flags);

    size_t nallocx(size_t size, int flags);

    int mallctl(const char *name, void *oldp, size_t *oldlenp, void *newp, size_t newlen);

    int mallctlnametomib(const char *name, size_t *mibp, size_t *miblenp);

    int mallctlbymib(const size_t *mib, size_t miblen, void *oldp, size_t *oldlenp, void *newp,
                     size_t newlen);

    void malloc_stats_print(void (*write_cb) (void *, const char *), void *cbopaque, const char *opts);

    size_t malloc_usable_size(const void *ptr);

    void (*malloc_message)(void *cbopaque, const char *s);

    const char *malloc_conf;

    
    //    当在有前缀条件下安装时，接口为：

    void *je_mallocx(size_t size, int flags);

    void *je_rallocx(void *ptr, size_t size, int flags);

    size_t je_xallocx(void *ptr, size_t size, size_t extra, int flags);

    size_t je_sallocx(void *ptr, int flags);

    void je_dallocx(void *ptr, int flags);

    void je_sdallocx(void *ptr, size_t size, int flags);

    size_t je_nallocx(size_t size, int flags);

    int je_mallctl(const char *name, void *oldp, size_t *oldlenp, void *newp, size_t newlen);

    int je_mallctlnametomib(const char *name, size_t *mibp, size_t *miblenp);

    int je_mallctlbymib(const size_t *mib, size_t miblen, void *oldp, size_t *oldlenp, void *newp,
                     size_t newlen);

    void je_malloc_stats_print(void (*write_cb) (void *, const char *), void *cbopaque, const char *opts);

    size_t je_malloc_usable_size(const void *ptr);


}

//MALLOCX_LG_ALIGN(la)
////    Align the memory allocation to start at an address that is a multiple of (1 << la). This macro does
////    not validate that la is within the valid range.
//
//MALLOCX_ALIGN(a)
////    Align the memory allocation to start at an address that is a multiple of a, where a is a power of
////    two. This macro does not validate that a is a power of 2.
//
//MALLOCX_ZERO
////    Initialize newly allocated memory to contain zero bytes. In the growing reallocation case, the real
////    size prior to reallocation defines the boundary between untouched bytes and those that are
////    initialized to contain zero bytes. If this macro is absent, newly allocated memory is uninitialized.
//
//MALLOCX_TCACHE(tc)
////    Use the thread-specific cache (tcache) specified by the identifier tc, which must have been acquired
////    via the tcache.create mallctl. This macro does not validate that tc specifies a valid identifier.
//
//MALLOCX_TCACHE_NONE
////    Do not use a thread-specific cache (tcache). Unless MALLOCX_TCACHE(tc) or MALLOCX_TCACHE_NONE is
////    specified, an automatically managed tcache will be used under many circumstances. This macro cannot
////    be used in the same flags argument as MALLOCX_TCACHE(tc).
//
//MALLOCX_ARENA(a)
////    Use the arena specified by the index a. This macro has no effect for regions that were allocated via
////    an arena other than the one specified. This macro does not validate that a specifies an arena index
////    in the valid range.

