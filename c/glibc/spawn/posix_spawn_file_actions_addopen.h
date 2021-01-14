#include <spawn.h>

//add close or open action to spawn
//file actions object (ADVANCED REALTIME)
int posix_spawn_file_actions_addclose(posix_spawn_file_actions_t *
      file_actions, int fildes);
int posix_spawn_file_actions_addopen(posix_spawn_file_actions_t *
      restrict file_actions, int fildes,
      const char *restrict path, int oflag, mode_t mode);

