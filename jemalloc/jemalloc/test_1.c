#include <stdio.h>

#ifdef USE_JEMALLOC
#include <jemalloc/jemalloc.h>
#endif

int main()
{
    int *I = malloc(1024);
    
    free(I);
}
