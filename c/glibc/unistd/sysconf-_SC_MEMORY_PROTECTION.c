/* Test sysconf, rongtao 2021年 02月 09日 星期二 14:40:49 CST */
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_MEMORY_PROTECTION = %ld\n", sysconf(_SC_MEMORY_PROTECTION));
    return 0;
}
