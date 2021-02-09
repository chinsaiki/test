/* Test sysconf, rongtao 2021年 02月 09日 星期二 14:47:09 CST */
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_SCHAR_MAX = %ld\n", sysconf(_SC_SCHAR_MAX));
    return 0;
}
