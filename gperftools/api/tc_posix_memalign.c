#include <gperftools/tcmalloc.h>
#include <stdio.h>
    
//void* tc_malloc(size_t size);
//void tc_free(void* ptr);
//
//size_t tc_malloc_size(void* ptr);
//void* tc_realloc(void* ptr, size_t size);
//void* tc_calloc(size_t nmemb, size_t size);
//void* tc_memalign(size_t __alignment, size_t __size);
int tc_posix_memalign(void** ptr, size_t align, size_t size);
//posix_memalign()  returns zero on success, or one of the error values listed in the next section on fail‐
//       ure.  Note that errno is not set.

int main()
{
    char *str;
    int ret = tc_posix_memalign((void**)&str, 8, 1024);
    printf("%d:%d\n", ret, tc_malloc_size(str));

    str = tc_realloc(str, 2024);

    printf("%d\n", tc_malloc_size(str));

    tc_free(str);
}
