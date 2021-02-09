//getresuid, getresgid - get real, effective and saved user/group IDs

#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <unistd.h>

int getresuid(uid_t *ruid, uid_t *euid, uid_t *suid);
//int getresgid(gid_t *rgid, gid_t *egid, gid_t *sgid);

