#include <sched.h>

//determine CPU on which the calling thread is running
int sched_getcpu(void);
