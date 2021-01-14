#include <spawn.h>

//destroy  and  initialize  spawn file
//actions object (ADVANCED REALTIME)
int posix_spawn_file_actions_destroy(posix_spawn_file_actions_t *    file_actions);
int posix_spawn_file_actions_init(posix_spawn_file_actions_t *    file_actions);

