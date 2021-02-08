#include <sys/types.h>

//get thread identification
pid_t gettid(void);


#include <syscall.h>

#define gettid() syscall(__NR_gettid)

