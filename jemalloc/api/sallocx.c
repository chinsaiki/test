#include <stdio.h>
#include <jemalloc/jemalloc.h>

int main()
{
    int *I = mallocx(1024, MALLOCX_LG_ALIGN(8));

    size_t s1 = sallocx(I, 0);

    printf("size = %d.\n", s1);
    
    free(I);
}
