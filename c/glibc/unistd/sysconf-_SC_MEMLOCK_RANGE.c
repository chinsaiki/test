/* Test sysconf, rongtao 2021年 02月 09日 星期二 14:40:16 CST */
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_MEMLOCK_RANGE = %ld\n", sysconf(_SC_MEMLOCK_RANGE));
    return 0;
}
