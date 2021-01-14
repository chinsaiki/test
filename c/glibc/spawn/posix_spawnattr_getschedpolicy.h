#include <spawn.h>
#include <sched.h>

//get  and  set  the  spawn-schedpolicy
//attribute of a spawn attributes object (ADVANCED REALTIME)
int posix_spawnattr_getschedpolicy(const posix_spawnattr_t *
      restrict attr, int *restrict schedpolicy);
int posix_spawnattr_setschedpolicy(posix_spawnattr_t *attr,
      int schedpolicy);

