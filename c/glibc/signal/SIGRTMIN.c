
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

//#define SIGRTMIN        (__libc_current_sigrtmin ())
//#define SIGRTMAX        (__libc_current_sigrtmax ())


int main(int argc, char *argv[])
{
    printf("SIGRTMIN = %d\n", SIGRTMIN);
    printf("SIGRTMAX = %d\n", SIGRTMAX);

}
