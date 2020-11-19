#include "ziplist.h"
#include <stdio.h>
#include <assert.h>
#include "zmalloc.h"
#include <sys/time.h>

int main(int argc, char **argv)
{
#ifdef REDIS_TEST
    ziplistTest(argc, argv);
#else
    printf("Define REDIS_TEST, compile again.\n");
#endif
}


