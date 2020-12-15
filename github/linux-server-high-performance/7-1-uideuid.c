#include <stdio.h>
#include <unistd.h>

#include <unistd.h>
#include <sys/types.h>

//getuid() returns the real user ID of the calling process.
uid_t getuid(void);

//geteuid() returns the effective user ID of the calling process.
uid_t geteuid(void);



int main(int argc, const char *argv[])
{
    uid_t  uid = getuid();
    uid_t  euid = geteuid();
    fprintf(stdout, "userid is %d, effective userid is: %d\n", uid, euid);
    return 0;
}
