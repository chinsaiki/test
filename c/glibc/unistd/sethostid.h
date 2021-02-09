//gethostid, sethostid - get or set the unique identifier of the current host

#include <unistd.h>

long gethostid(void);
int sethostid(long hostid);

