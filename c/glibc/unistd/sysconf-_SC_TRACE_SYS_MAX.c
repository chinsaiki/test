/* Test sysconf, rongtao 2021年 02月 09日 星期二 15:11:41 CST */
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_TRACE_SYS_MAX = %ld\n", sysconf(_SC_TRACE_SYS_MAX));
    return 0;
}
