#include <stdio.h>
#include <jemalloc/jemalloc.h>

int main()
{
    int *I = je_mallocx(1024, MALLOCX_LG_ALIGN(8));

    size_t s1 = je_sallocx(I, 0);

    printf("size = %d.\n", s1);
    
    je_free(I);
}




