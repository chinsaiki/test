#include <unistd.h>

//ttyname, ttyname_r - return name of a terminal
char *ttyname(int fd);

int ttyname_r(int fd, char *buf, size_t buflen);

