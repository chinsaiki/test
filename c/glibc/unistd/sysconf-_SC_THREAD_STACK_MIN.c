

#include <stdio.h>
#include <unistd.h>

int main()
{
    printf("_SC_THREAD_STACK_MIN = %d\n", sysconf(_SC_THREAD_STACK_MIN));
}


