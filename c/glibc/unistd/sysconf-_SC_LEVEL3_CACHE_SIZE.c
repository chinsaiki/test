/* Test sysconf, rongtao 2021年 02月 09日 星期二 15:09:53 CST */
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_LEVEL3_CACHE_SIZE = %ld\n", sysconf(_SC_LEVEL3_CACHE_SIZE));
    return 0;
}
