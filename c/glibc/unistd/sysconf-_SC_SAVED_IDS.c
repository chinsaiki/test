/* Test sysconf, rongtao 2021年 02月 09日 星期二 14:31:50 CST */
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_SAVED_IDS = %d\n", sysconf(_SC_SAVED_IDS));
    return 0;
}
