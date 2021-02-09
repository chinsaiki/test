/* Test sysconf, rongtao 2021年 02月 09日 星期二 14:49:12 CST */
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_LEVEL1_ICACHE_ASSOC = %ld\n", sysconf(_SC_LEVEL1_ICACHE_ASSOC));
    return 0;
}
