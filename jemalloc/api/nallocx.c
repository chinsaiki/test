#include <stdio.h>
#include <jemalloc/jemalloc.h>

int main()
{
    size_t s1 = nallocx(1024, MALLOCX_LG_ALIGN(8));
    
    printf("size = %d.\n", s1);
    
//    free(I);
}

