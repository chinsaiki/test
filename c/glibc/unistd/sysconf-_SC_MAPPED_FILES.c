/* Test sysconf, rongtao 2021年 02月 09日 星期二 14:39:37 CST */
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_MAPPED_FILES = %ld\n", sysconf(_SC_MAPPED_FILES));
    return 0;
}
