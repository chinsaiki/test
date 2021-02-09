/* Test sysconf, rongtao 2021年 02月 09日 星期二 14:24:40 CST */
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_CLK_TCK = %d\n", sysconf(_SC_CLK_TCK));
    return 0;
}
