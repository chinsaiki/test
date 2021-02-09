/* Test sysconf, rongtao 2021年 02月 09日 星期二 14:37:35 CST */
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_ASYNCHRONOUS_IO = %d\n", sysconf(_SC_ASYNCHRONOUS_IO));
    return 0;
}
