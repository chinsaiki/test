#include <sys/types.h>
#include "zmalloc.h"


int zmalloc_test(int argc, char **argv) 
{
    void *ptr;
    size_t allocated, active, resident;
    
    printf("Initial used memory: %zu\n", zmalloc_get_rss());
    ptr = zmalloc(123);


    zmalloc_get_allocator_info(&allocated, &active, &resident);


    printf("allocated = %d, active = %d, resident = %d\n", allocated, active, resident);
    
    return 0;
}

int main(int argc, char **argv)
{
    zmalloc_test(argc, argv);
}


