#include <spawn.h>

//posix_spawn, posix_spawnp - spawn a process (ADVANCED REALTIME) 产生/孵化一个进程
int posix_spawn(pid_t *restrict pid, const char *restrict path,
      const posix_spawn_file_actions_t *file_actions,
      const posix_spawnattr_t *restrict attrp,
      char *const argv[restrict], char *const envp[restrict]);
int posix_spawnp(pid_t *restrict pid, const char *restrict file,
      const posix_spawn_file_actions_t *file_actions,
      const posix_spawnattr_t *restrict attrp,
      char *const argv[restrict], char * const envp[restrict]);

