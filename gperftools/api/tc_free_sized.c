#include <gperftools/tcmalloc.h>
#include <stdio.h>

void* tc_malloc(size_t size);
void tc_free(void* ptr);
void tc_free_sized(void *ptr, size_t size);


int main()
{
    char *str = tc_malloc(1024);
    char *str2 = tc_malloc(2041);
    printf("%s\n", str);

    tc_free_sized(str, 1024);
    
    tc_free(str);
    
    printf("%d\n", tc_malloc_size(str));
    printf("%d\n", tc_malloc_size(str2));

    *(str+524) = 'A';
}


