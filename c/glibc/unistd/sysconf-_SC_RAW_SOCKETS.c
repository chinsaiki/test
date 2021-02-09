/* Test sysconf, rongtao 2021年 02月 09日 星期二 15:10:55 CST */
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_RAW_SOCKETS = %ld\n", sysconf(_SC_RAW_SOCKETS));
    return 0;
}
