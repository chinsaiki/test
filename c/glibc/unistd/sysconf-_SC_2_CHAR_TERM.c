/* Test sysconf, rongtao 2021年 02月 09日 星期二 14:45:56 CST */
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_2_CHAR_TERM = %ld\n", sysconf(_SC_2_CHAR_TERM));
    return 0;
}
