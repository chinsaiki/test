#include <gperftools/tcmalloc.h>
#include <gperftools/nallocx.h>
#include <stdio.h>

/*
 * The nallocx function allocates no memory, but it performs the same size
 * computation as the malloc function, and returns the real size of the
 * allocation that would result from the equivalent malloc function call.
 * nallocx is a malloc extension originally implemented by jemalloc:
 * http://www.unix.com/man-page/freebsd/3/nallocx/
 *
 * Note, we only support MALLOCX_LG_ALIGN flag and nothing else.
 */
size_t nallocx(size_t size, int flags);


int main()
{
    size_t size = nallocx(1024, MALLOCX_LG_ALIGN(1024));
    printf("%d\n", size);

}

