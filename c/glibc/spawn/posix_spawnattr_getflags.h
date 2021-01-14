#include <spawn.h>

//get  and set the spawn-flags attribute of a spawn
//attributes object (ADVANCED REALTIME)
int posix_spawnattr_getflags(const posix_spawnattr_t *restrict attr,
      short *restrict flags);
int posix_spawnattr_setflags(posix_spawnattr_t *attr, short flags);

