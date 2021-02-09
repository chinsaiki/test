/* Test sysconf, rongtao 2021年 02月 09日 星期二 14:47:54 CST */
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_FIFO = %ld\n", sysconf(_SC_FIFO));
    return 0;
}
