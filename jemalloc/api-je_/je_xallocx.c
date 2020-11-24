#include <stdio.h>
#include <jemalloc/jemalloc.h>

void test_xallocx()
{
    size_t s1;
    int *I = je_mallocx(1024, MALLOCX_LG_ALIGN(8));

    s1 = je_sallocx(I, 0);
    printf("size = %d.\n", s1);

    s1 = je_xallocx(I, 1024, 1024, MALLOCX_LG_ALIGN(8));
    printf("size = %d.\n", s1);
    s1 = je_sallocx(I, 0);
    printf("size = %d.\n", s1);

    s1 = je_xallocx(I, 2024, 0, MALLOCX_LG_ALIGN(8));
    printf("size = %d.\n", s1);
    s1 = je_sallocx(I, 0);
    printf("size = %d.\n", s1);
    
    je_free(I);
}



int main()
{
    test_xallocx();
}
