#include <stdio.h>

#include <jemalloc/jemalloc.h>

void test_rallocx()
{
    int i;
    size_t s1;
    int *I = je_mallocx(1024, MALLOCX_LG_ALIGN(8));
    s1 = je_malloc_usable_size(I);
    printf("size = %d.\n", s1);

    I = je_rallocx(I, 2024, MALLOCX_LG_ALIGN(8));

    s1 = je_malloc_usable_size(I);
    printf("size = %d.\n", s1);

    I = je_rallocx(I, 252, MALLOCX_LG_ALIGN(8));

    s1 = je_malloc_usable_size(I);
    printf("size = %d.\n", s1);

    
}


int main()
{
    test_rallocx();
}


