#include "zmalloc.h"

#define UNUSED(x) ((void)(x))
int zmalloc_test(int argc, char **argv) {
    void *ptr;

    UNUSED(argc);
    UNUSED(argv);
    printf("Initial used memory: %zu\n", zmalloc_used_memory());
    ptr = zmalloc(123);
    printf("zmalloc_size: %zu\n", zmalloc_size(ptr));
    printf("zmalloc_usable: %zu\n", zmalloc_usable(ptr));
    ptr = zrealloc(ptr, 1025);
    printf("zmalloc_size: %zu\n", zmalloc_size(ptr));
    printf("zmalloc_usable: %zu\n", zmalloc_usable(ptr));
    zfree(ptr);
    printf("zmalloc_size: %zu\n", zmalloc_size(ptr));
    printf("zmalloc_usable: %zu\n", zmalloc_usable(ptr));
    return 0;
}

int main(int argc, char **argv)
{
    zmalloc_test(argc, argv);
}


