//fchownat - change ownership of a file relative to a directory file descriptor

#include <fcntl.h> /* Definition of AT_* constants */
#include <unistd.h>

int fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group, int flags);

