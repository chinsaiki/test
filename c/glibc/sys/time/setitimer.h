#include <sys/time.h>

//到时后，触发 SIGALRM
//getitimer, setitimer - get or set value of an interval timer
//int getitimer(int which, struct itimerval *curr_value);
int setitimer(int which, const struct itimerval *new_value,
                struct itimerval *old_value);


/*
 * Names of the interval timers, and structure
 * defining a timer setting.
 */
#define	ITIMER_REAL	0
#define	ITIMER_VIRTUAL	1
#define	ITIMER_PROF	2

//ITIMER_REAL    decrements in real time, and delivers SIGALRM upon expiration.
//
//ITIMER_VIRTUAL decrements only when the process is executing, and delivers SIGVTALRM upon expiration.
//
//ITIMER_PROF    decrements both when the process executes and when the system is executing  on  behalf  of
//              the  process.  Coupled with ITIMER_VIRTUAL, this timer is usually used to profile the time
//              spent by the application in user and kernel space.  SIGPROF is delivered upon expiration.


