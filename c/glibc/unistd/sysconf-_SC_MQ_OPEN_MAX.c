/* Test sysconf, rongtao 2021年 02月 09日 星期二 14:41:14 CST */
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_MQ_OPEN_MAX = %ld\n", sysconf(_SC_MQ_OPEN_MAX));
    return 0;
}
