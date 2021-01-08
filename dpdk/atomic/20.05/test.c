#include <stdio.h>

#include "rte_atomic.h"

void test_atomic16()
{
    uint16_t dst = 1;

    rte_atomic16_cmpset(&dst, 2, 2); //相同时，交换，否则不交换
    printf("%d\n", dst);

    rte_atomic16_exchange(&dst, 3);
    printf("%d\n", dst);
}
void test_atomic32()
{
    uint32_t dst = 1;

    rte_atomic32_cmpset(&dst, 2, 2); //相同时，交换，否则不交换
    printf("%d\n", dst);

    rte_atomic32_exchange(&dst, 3);
    printf("%d\n", dst);
}
void test_atomic64()
{
    uint64_t dst = 1;

    rte_atomic64_cmpset(&dst, 2, 2); //相同时，交换，否则不交换
    printf("%d\n", dst);

    rte_atomic64_exchange(&dst, 3);
    printf("%d\n", dst);
}

int main()
{
    test_atomic16();
    test_atomic32();
    test_atomic64();
}
