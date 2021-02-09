/* Test sysconf, rongtao 2021年 02月 09日 星期二 14:31:38 CST */
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_JOB_CONTROL = %d\n", sysconf(_SC_JOB_CONTROL));
    return 0;
}
