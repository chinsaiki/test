#include <stdio.h>
#include "hpmalloc.h"


module_id_t mod1 = 1;
module_id_t mod2 = 2;


void test_version() 
{
    printf("Version         %s\n", hpmalloc_version());
    printf("Physic memory   %zu\n", hpmalloc_get_memory_size());
}


void test_used()
{
    void *ptr;
    
    printf("Init:           %zu\n", hpmalloc_used_memory());
    ptr = hpmalloc(12300, mod1);
    printf("Allocated:      %zu\n", hpmalloc_used_memory());
    ptr = hprealloc(ptr, 31744001, mod1);
    printf("Reallocated:    %zu, %zu\n", hpmalloc_used_memory(), hpmalloc_module_used_memory(mod1));
    hpfree(ptr);
    printf("Freed:          %zu\n", hpmalloc_used_memory());
}

void test_jemalloc()
{
    size_t allocated, active, resident;
    hpmalloc_jemalloc_purge();
    
    int ret = hpmalloc_get_allocator_info(&allocated, &active, &resident);

    printf("allocated = %zu, active = %zu, resident = %zu\n", allocated, active, resident);
}

void test_check()
{
    char *ptr = hpmalloc(64, mod1);
    
    *(char*)(ptr-1) = 1;
    
    printf("%s\n", hpmalloc_strerror(hpmemcheck(ptr)));
    
    hpfree(ptr);
}


void test_multi_thread()
{
    static unsigned long moduleID[20] ={ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20 };
    
}

int main(int argc, char **argv)
{
//    test_version();
//    test_used();
//    test_jemalloc();
//    test_check();

}

