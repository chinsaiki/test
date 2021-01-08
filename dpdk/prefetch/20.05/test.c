#include <malloc.h>

#include "rte_prefetch.h"

int main()
{
    char *str = malloc(1024);

    rte_prefetch0(str);

    free(str);
}
