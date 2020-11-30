#include <gperftools/tcmalloc.h>
#include <stdio.h>
    
//void* tc_malloc(size_t size);
//void tc_free(void* ptr);
//
//size_t tc_malloc_size(void* ptr);
//void* tc_realloc(void* ptr, size_t size);
void* tc_calloc(size_t nmemb, size_t size);

int main()
{
    char *str = tc_calloc(1024, 1);
    printf("%d\n", tc_malloc_size(str));

    str = tc_realloc(str, 2024);

    printf("%d\n", tc_malloc_size(str));

    tc_free(str);
}



