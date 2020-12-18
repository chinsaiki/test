#include <stdio.h>
#include "common.h"

extern int execve2(char *path, char *argv[], char *envp[]);
 
int main()
{
	char *argv[] = {"/bin/ls", "-al", "test", NULL};
    debug("start.\n");
	execve2(argv[0], argv, NULL);
    debug("end.\n");
	return 0;
}

