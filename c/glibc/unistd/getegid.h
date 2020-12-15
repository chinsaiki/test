#include <unistd.h>
#include <sys/types.h>

//getuid() returns the real user ID of the calling process.
uid_t getuid(void);

//geteuid() returns the effective user ID of the calling process.
uid_t geteuid(void);

