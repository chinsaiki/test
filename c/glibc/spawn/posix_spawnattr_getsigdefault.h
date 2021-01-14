#include <signal.h>
#include <spawn.h>

//get and set the spawn-sigdefault attribute
//of a spawn attributes object (ADVANCED REALTIME)
int posix_spawnattr_getsigdefault(const posix_spawnattr_t *
      restrict attr, sigset_t *restrict sigdefault);
int posix_spawnattr_setsigdefault(posix_spawnattr_t *restrict attr,
      const sigset_t *restrict sigdefault);

