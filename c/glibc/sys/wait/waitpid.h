#include <sys/types.h>
#include <sys/wait.h>

pid_t wait(int *status);

//wait for process to change state
//The value of pid can be:
//< -1   meaning wait for any child process whose process group ID is equal to the absolute value of pid.
//-1     meaning wait for any child process.
//0      meaning wait for any child process whose process group ID is equal to that of the calling process.
//> 0    meaning wait for the child whose process ID is equal to the value of pid.

pid_t waitpid(pid_t pid, int *status, int options);

/* Wait for a childing matching IDTYPE and ID to change the status and
   place appropriate information in *INFOP.
   If IDTYPE is P_PID, match any process whose process ID is ID.
   If IDTYPE is P_PGID, match any process whose process group is ID.
   If IDTYPE is P_ALL, match any process.
   If the WNOHANG bit is set in OPTIONS, and that child
   is not already dead, clear *INFOP and return 0.  If successful, store
   exit code and status in *INFOP.

   This function is a cancellation point and therefore not marked with
   __THROW.  */
int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options);

