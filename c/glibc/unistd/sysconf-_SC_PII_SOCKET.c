/* Test sysconf, rongtao 2021年 02月 09日 星期二 14:41:59 CST */
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_PII_SOCKET = %ld\n", sysconf(_SC_PII_SOCKET));
    return 0;
}
