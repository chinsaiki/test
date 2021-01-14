#include <signal.h>
#include <spawn.h>

//get  and set the spawn-sigmask attribute of a
//spawn attributes object (ADVANCED REALTIME)
int posix_spawnattr_getsigmask(const posix_spawnattr_t *restrict attr,
      sigset_t *restrict sigmask);
int posix_spawnattr_setsigmask(posix_spawnattr_t *restrict attr,
      const sigset_t *restrict sigmask);

