#include <libvmem.h>

VMEM *vmem_create(const char *dir, size_t size);
//VMEM *vmem_create_in_region(void *addr, size_t size);
//void vmem_delete(VMEM *vmp);
//int vmem_check(VMEM *vmp);
//void vmem_stats_print(VMEM *vmp, const char *opts);

int main()
{
    
    VMEM *vmem = vmem_create("./", VMEM_MIN_POOL);

    void *str = vmem_malloc(vmem, 1024);

    vmem_free(vmem, str);
}
