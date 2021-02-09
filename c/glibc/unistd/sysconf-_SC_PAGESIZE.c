/* Test sysconf, rongtao 2021年 02月 09日 星期二 14:27:05 CST */
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_PAGESIZE = %d\n", sysconf(_SC_PAGESIZE));
    return 0;
}
