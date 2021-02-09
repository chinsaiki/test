/* Test sysconf, rongtao 2021年 02月 09日 星期二 14:37:08 CST */
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_PRIORITY_SCHEDULING = %d\n", sysconf(_SC_PRIORITY_SCHEDULING));
    return 0;
}
