#include <spawn.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc ,char *argv[])
{
	int child_pid[2];
    int ret;
    int wait_val[2];

    ret = posix_spawn(&child_pid[0], "/bin/ls", NULL, NULL, argv, NULL);
    if(ret != 0) {
        printf("posix_spawn error.\n");
        exit(EXIT_FAILURE);
    }
    ret = posix_spawn(&child_pid[1], "/bin/cat", NULL, NULL, argv, NULL);
    if(ret != 0) {
        printf("posix_spawn error.\n");
        exit(EXIT_FAILURE);
    }
    
    wait(&wait_val[0]);
    printf("child %d, wait value = %d\n", child_pid[0], wait_val[0]);
    
    wait(&wait_val[1]);
    printf("child %d, wait value = %d\n", child_pid[1], wait_val[1]);
}


