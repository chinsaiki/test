#include <stdio.h>
#include "rte_jhash.h"

int main(int argc, char *argv[])
{
    if(argc < 3) {
        printf("usage: %s [keys] [initval].\n",argv[0]);
        return 0;
    }

    char *keys = argv[1];
    uint32_t initval = atoi(argv[2]);

    printf("rte_jhash(%s) = %u\n", keys, rte_jhash(keys, strlen(keys), initval));
}
