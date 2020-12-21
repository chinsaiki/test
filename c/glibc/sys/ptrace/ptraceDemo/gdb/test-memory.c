#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <unistd.h>
#include <errno.h>

#include "debuglib.h"

void print_hello()
{
    printf("Hello.\n");
}


int main(int argc, char** argv)
{
    debug_breakpoint *bp = create_breakpoint(getpid(), &print_hello);

    print_hello();
//    a = 1;

//    dump_process_memory(getpid(), (unsigned)&a, (unsigned)&a);
    
    //TODO
}

