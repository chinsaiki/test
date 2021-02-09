/* Test sysconf, rongtao 2021年 02月 09日 星期二 14:14:20 CST */
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_PHYS_PAGES = %d\n", sysconf(_SC_PHYS_PAGES));
    return 0;
}
