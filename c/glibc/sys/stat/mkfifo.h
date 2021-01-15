#include <sys/types.h>
#include <sys/stat.h>

//make a FIFO special file (a named pip)
int mkfifo(const char *pathname, mode_t mode);

