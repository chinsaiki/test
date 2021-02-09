/* Test sysconf, rongtao 2021年 02月 09日 星期二 14:45:20 CST */
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_XOPEN_UNIX = %ld\n", sysconf(_SC_XOPEN_UNIX));
    return 0;
}
