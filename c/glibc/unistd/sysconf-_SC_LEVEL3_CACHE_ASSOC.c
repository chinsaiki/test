/* Test sysconf, rongtao 2021年 02月 09日 星期二 15:09:47 CST */
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_LEVEL3_CACHE_ASSOC = %ld\n", sysconf(_SC_LEVEL3_CACHE_ASSOC));
    return 0;
}
