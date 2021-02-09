//fexecve - execute program specified via file descriptor

#include <unistd.h>

int fexecve(int fd, char *const argv[], char *const envp[]);

