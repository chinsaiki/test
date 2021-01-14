#include <spawn.h>

//get and set the spawn-pgroup attribute of a spawn
//attributes object (ADVANCED REALTIME)
int posix_spawnattr_getpgroup(const posix_spawnattr_t *restrict attr,
      pid_t *restrict pgroup);
int posix_spawnattr_setpgroup(posix_spawnattr_t *attr, pid_t pgroup);

