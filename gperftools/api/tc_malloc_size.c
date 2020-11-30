#include <gperftools/tcmalloc.h>
#include <stdio.h>
    
void* tc_malloc(size_t size);
void tc_free(void* ptr);

size_t tc_malloc_size(void* ptr);

int main()
{
    char *str = tc_malloc(1024);
    printf("%d\n", tc_malloc_size(str));

    tc_free(str);
}

