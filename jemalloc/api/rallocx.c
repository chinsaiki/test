#include <stdio.h>
#include <jemalloc/jemalloc.h>

void test_rallocx()
{
    int i;
    size_t s1;
    int *I = mallocx(1024, MALLOCX_LG_ALIGN(8));
    s1 = malloc_usable_size(I);
    printf("size = %d.\n", s1);

    I = rallocx(I, 2024, MALLOCX_LG_ALIGN(8));

    s1 = malloc_usable_size(I);
    printf("size = %d.\n", s1);

    I = rallocx(I, 252, MALLOCX_LG_ALIGN(8));

    s1 = malloc_usable_size(I);
    printf("size = %d.\n", s1);

    
}


int main()
{
    test_rallocx();
}

