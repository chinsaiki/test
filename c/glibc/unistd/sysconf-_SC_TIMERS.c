/* Test sysconf, rongtao 2021年 02月 09日 星期二 14:37:17 CST */
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_TIMERS = %d\n", sysconf(_SC_TIMERS));
    return 0;
}
