#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <unistd.h>

//setresuid, setresgid - set real, effective and saved user or group ID
//int setresuid(uid_t ruid, uid_t euid, uid_t suid);
int setresgid(gid_t rgid, gid_t egid, gid_t sgid);


