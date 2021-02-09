/* Test sysconf, rongtao 2021年 02月 09日 星期二 14:21:32 CST */
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_CHILD_MAX = %d\n", sysconf(_SC_CHILD_MAX));
    return 0;
}
