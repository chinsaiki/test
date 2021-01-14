#include <spawn.h>

//add dup2 action to spawn file actions object (ADVANCED REALTIME)
int posix_spawn_file_actions_adddup2(posix_spawn_file_actions_t *file_actions, int fildes, int newfildes);

