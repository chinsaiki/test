#include <signal.h>

//examine and change mask of blocked signals
int pthread_sigmask(int how, const sigset_t *set, sigset_t *oldset);


