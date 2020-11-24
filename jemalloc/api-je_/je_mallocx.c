#include <stdio.h>
#include <jemalloc/jemalloc.h>

void test_malloc()
{
    int i;
    size_t s1;
    int *I = je_malloc(1024);

    for(i=0;i<1024;i++)
        je_free(I);
    
    s1 = je_malloc_usable_size(I);
    printf("size = %d.\n", s1);
}

void test_mallocx()
{
    int i;
    size_t s1;
    int *I = je_mallocx(1024, MALLOCX_LG_ALIGN(8));

    for(i=0;i<1024;i++)
        je_free(I);
    
    s1 = malloc_usable_size(I);
    printf("size = %d.\n", s1);

}


int main()
{
    test_malloc();
    test_mallocx();
}
