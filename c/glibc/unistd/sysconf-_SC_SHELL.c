/* Test sysconf, rongtao 2021年 02月 09日 星期二 14:48:13 CST */
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_SHELL = %ld\n", sysconf(_SC_SHELL));
    return 0;
}
