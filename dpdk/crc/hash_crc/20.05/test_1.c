#include <stdio.h>
#include "rte_hash_crc.h"



int main()
{
    char buf[64] = {'A'};
    

    uint32_t crc = rte_hash_crc(buf, 64, 1);

    printf("buf = %x\n", crc);
}

