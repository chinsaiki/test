/* Test sysconf, rongtao 2021年 02月 09日 星期二 14:19:51 CST */
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_ARG_MAX = %d\n", sysconf(_SC_ARG_MAX));
    return 0;
}
