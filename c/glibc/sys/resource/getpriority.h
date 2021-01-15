#include <sys/time.h>
#include <sys/resource.h>

//get/set program scheduling priority
int getpriority(int which, int who);
int setpriority(int which, int who, int prio);

