#include <spawn.h>

//destroy and initialize spawn attributes object (ADVANCED REALTIME)
int posix_spawnattr_destroy(posix_spawnattr_t *attr);
int posix_spawnattr_init(posix_spawnattr_t *attr);

