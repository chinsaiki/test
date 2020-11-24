#include <sys/types.h>
#include "zmalloc.h"


int zmalloc_test(int argc, char **argv) 
{
    void *ptr;
    size_t allocated, active, resident;
    
    printf("Initial used memory: %zu\n", zmalloc_get_rss());
    ptr = zmalloc(123);

    size_t size = zmalloc_get_memory_size();

    printf("zmalloc_get_memory_size = %ld bytes, %ld Mb, %ld Gb\n", size, size/1024, size/1024/1024);

    zfree(ptr);

    return 0;
}

int main(int argc, char **argv)
{
    zmalloc_test(argc, argv);
}
