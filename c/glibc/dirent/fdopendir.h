#include <sys/types.h>
#include <dirent.h>

//open a directory
DIR *opendir(const char *name);
DIR *fdopendir(int fd);

//参见 test/linux-5.6.5/tools/testing/selftests/proc/read.c
