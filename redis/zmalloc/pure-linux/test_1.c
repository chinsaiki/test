#include "zmalloc.h"


void test0(int argc, char **argv) 
{
    size_t phy_size = zmalloc_get_memory_size();

    printf("zmalloc version %s\n", zmalloc_version());
    printf("Physic memory: %zu\n", phy_size);
}


void test1(int argc, char **argv) 
{
    void *ptr;
    
    printf("Initial used memory: %zu\n", zmalloc_used_memory());
    ptr = zmalloc(123);
    printf("Allocated 123 bytes; used: %zu\n", zmalloc_used_memory());
    ptr = zrealloc(ptr, 456);
    printf("Reallocated to 456 bytes; used: %zu\n", zmalloc_used_memory());
    zfree(ptr);
    printf("Freed pointer; used: %zu\n", zmalloc_used_memory());
}

void test2_jemalloc(int argc, char **argv) 
{
    size_t allocated, active, resident;
    int ret = zmalloc_get_allocator_info(&allocated, &active, &resident);

    printf("allocated = %zu, active = %zu, resident = %zu\n", allocated, active, resident);
}

int main(int argc, char **argv)
{
    test0(argc, argv);
    test1(argc, argv);
    test2_jemalloc(argc, argv);
}
