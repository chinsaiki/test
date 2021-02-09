/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

#include <signal.h>
#include <time.h>
#include <stdio.h>

//struct timespec {
//   time_t tv_sec;                /* Seconds */
//   long   tv_nsec;               /* Nanoseconds */
//};
//
//struct itimerspec {
//   struct timespec it_interval;  /* Timer interval */
//   struct timespec it_value;     /* Initial expiration */
//};


int main()
{
    struct itimerspec it;
    timer_t timerid;
    struct sigevent sev;

    //file sev
    
    timer_create(CLOCK_REALTIME, &sev, &timerid);
        
    timer_gettime(timerid, &it);
}


