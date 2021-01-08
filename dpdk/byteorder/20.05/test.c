#include <stdio.h>

#include "rte_byteorder.h"

void test_order16()
{
    uint16_t val = 0x1234;
    
    printf("0x%x\n", rte_arch_bswap16(val));
    printf("0x%x\n", rte_bswap16(val));
    
}
void test_order32()
{
    uint32_t val = 0x12345678;
    
    printf("0x%x\n", rte_arch_bswap32(val));
    printf("0x%x\n", rte_bswap32(val));

}
void test_order64()
{
    uint64_t val = 0x1234567890119911;
    
    printf("0x%lx\n", rte_arch_bswap64(val));
    printf("0x%lx\n", rte_bswap64(val));

}

int main()
{
    test_order16();
    test_order32();
    test_order64();
}
