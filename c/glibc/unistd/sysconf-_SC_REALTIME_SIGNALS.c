/* Test sysconf, rongtao 2021年 02月 09日 星期二 14:34:54 CST */
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_REALTIME_SIGNALS = %d\n", sysconf(_SC_REALTIME_SIGNALS));
    return 0;
}
