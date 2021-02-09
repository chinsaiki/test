/* Test sysconf, rongtao 2021年 02月 09日 星期二 14:49:00 CST */
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_LEVEL1_ICACHE_SIZE = %ld\n", sysconf(_SC_LEVEL1_ICACHE_SIZE));
    return 0;
}
