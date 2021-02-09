/* Test sysconf, rongtao 2021年 02月 09日 星期二 14:27:19 CST */
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_PAGE_SIZE = %d\n", sysconf(_SC_PAGE_SIZE));
    return 0;
}
