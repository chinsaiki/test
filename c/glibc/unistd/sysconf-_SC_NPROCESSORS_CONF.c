/* Test sysconf, rongtao 2021年 02月 09日 星期二 14:18:04 CST */
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_NPROCESSORS_CONF = %d\n", sysconf(_SC_NPROCESSORS_CONF));
    return 0;
}
