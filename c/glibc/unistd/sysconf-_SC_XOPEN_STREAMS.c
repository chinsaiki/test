/* Test sysconf, rongtao 2021年 02月 09日 星期二 15:11:52 CST */
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_XOPEN_STREAMS = %ld\n", sysconf(_SC_XOPEN_STREAMS));
    return 0;
}
