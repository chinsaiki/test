#include <stdio.h>
#include <string.h>
#include <libvmem.h>

//VMEM *vmem_create(const char *dir, size_t size);
//VMEM *vmem_create_in_region(void *addr, size_t size);
//void vmem_delete(VMEM *vmp);
//int vmem_check(VMEM *vmp);
void vmem_stats_print(VMEM *vmp, const char *opts);


int main()
{
    char *buffer;
    posix_memalign(&buffer, getpagesize(), VMEM_MIN_POOL*2);
    memset(buffer, 'A', VMEM_MIN_POOL*2);

    
    VMEM *vmem = vmem_create_in_region(buffer, VMEM_MIN_POOL);

    char *str = vmem_malloc(vmem, 1024);

    printf("str = %s\n", str);

    vmem_stats_print(vmem, "gmabl");

    vmem_free(vmem, str);

    vmem_delete(vmem);
}


