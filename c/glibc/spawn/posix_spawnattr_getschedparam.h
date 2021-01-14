#include <spawn.h>
#include <sched.h>

//get and set the spawn-schedparam attribute
//of a spawn attributes object (ADVANCED REALTIME)
int posix_spawnattr_getschedparam(const posix_spawnattr_t *
      restrict attr, struct sched_param *restrict schedparam);
int posix_spawnattr_setschedparam(posix_spawnattr_t *restrict attr,
        const struct sched_param *restrict schedparam);

