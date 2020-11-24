#include <sys/types.h>
#include "zmalloc.h"


int zmalloc_test(int argc, char **argv) 
{
    void *ptr;
    size_t allocated, active, resident;
    
    printf("Initial used memory: %zu\n", zmalloc_get_rss());
    ptr = zmalloc(123);

    size_t size = zmalloc_get_private_dirty(-1);

    printf("size = %d\n", size);

    zfree(ptr);

    return 0;
}

int main(int argc, char **argv)
{
    zmalloc_test(argc, argv);
}


