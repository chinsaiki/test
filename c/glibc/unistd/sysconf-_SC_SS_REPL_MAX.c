/* Test sysconf, rongtao 2021年 02月 09日 星期二 15:11:08 CST */
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_SS_REPL_MAX = %ld\n", sysconf(_SC_SS_REPL_MAX));
    return 0;
}
