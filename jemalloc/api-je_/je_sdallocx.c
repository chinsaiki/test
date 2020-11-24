#include <stdio.h>
#include <jemalloc/jemalloc.h>

int main()
{
    int *I = je_mallocx(1024, MALLOCX_LG_ALIGN(8));

    size_t s1 = je_sallocx(I, 0);
    printf("size = %d.\n", s1);
    
    je_sdallocx(I, 256, MALLOCX_ZERO);

//    size_t s2 = je_xallocx(I, 1024, 1024, MALLOCX_ZERO);
//    printf("size = %d.\n", s2);
    
    size_t s3 = je_sallocx(I, 0);
    printf("size = %d.\n", s3);
    
    je_free(I);
}
