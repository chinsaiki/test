//DEFAULT_MMAP_THRESHOLD_MAX:
//The lower limit for this parameter is 0.  The upper limit is DEFAULT_MMAP_THRESHOLD_MAX:  512*1024
//              on 32-bit systems or 4*1024*1024*sizeof(long) on 64-bit systems.

#include <stdio.h>
#include <malloc.h>

int main()
{
    printf("DEFAULT_MMAP_THRESHOLD_MAX = %ld\n", DEFAULT_MMAP_THRESHOLD_MAX);
}
