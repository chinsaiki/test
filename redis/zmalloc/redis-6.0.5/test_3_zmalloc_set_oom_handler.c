#include "zmalloc.h"

#define UNUSED(x) ((void)(x))
int zmalloc_test(int argc, char **argv) {
    void *ptr;

    UNUSED(argc);
    UNUSED(argv);
    printf("Initial used memory: %zu\n", zmalloc_used_memory());
    ptr = zmalloc(123);
    printf("Allocated 123 bytes; used: %zu\n", zmalloc_used_memory());
    ptr = zrealloc(ptr, 1025);
    printf("Reallocated to 456 bytes; used: %zu\n", zmalloc_used_memory());
    zfree(ptr);
    printf("Freed pointer; used: %zu\n", zmalloc_used_memory());

    
    ptr = zmalloc(1000000000000000);
    printf("zmalloc used: %zu\n", zmalloc_used_memory());
    
    return 0;
}

void out_of_memory_handler(size_t size)
{
    printf("Out of memory.\n");
}

int main(int argc, char **argv)
{
    zmalloc_set_oom_handler(out_of_memory_handler);

    zmalloc_test(argc, argv);
}

