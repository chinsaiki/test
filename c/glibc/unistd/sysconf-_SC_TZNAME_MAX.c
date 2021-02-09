/* Test sysconf, rongtao 2021年 02月 09日 星期二 14:31:27 CST */
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_TZNAME_MAX = %d\n", sysconf(_SC_TZNAME_MAX));
    return 0;
}
