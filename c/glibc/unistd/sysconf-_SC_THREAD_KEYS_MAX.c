/* Test sysconf, rongtao 2021年 02月 09日 星期二 14:43:00 CST */
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("_SC_THREAD_KEYS_MAX = %ld\n", sysconf(_SC_THREAD_KEYS_MAX));
    return 0;
}
