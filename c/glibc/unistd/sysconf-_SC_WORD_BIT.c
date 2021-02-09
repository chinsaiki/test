/* Test sysconf, rongtao 2021年 02月 09日 星期二 14:46:43 CST */
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_WORD_BIT = %ld\n", sysconf(_SC_WORD_BIT));
    return 0;
}
