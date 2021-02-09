//tcgetpgrp, tcsetpgrp - get and set terminal foreground process group

#include <unistd.h>

pid_t tcgetpgrp(int fd);

int tcsetpgrp(int fd, pid_t pgrp);

