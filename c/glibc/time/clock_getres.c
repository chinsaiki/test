/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

#include <signal.h>
#include <time.h>
#include <stdio.h>


//The  function  clock_getres() finds the resolution (precision) of the specified clock clk_id, and, if res
//       is non-NULL, stores it in the struct timespec pointed to by res.  

#define test(clk_id) {\
    struct timespec tp;\
    clock_getres(clk_id, &tp);\
    printf("resolution >> %40s: %ld, %ld\n", #clk_id, tp.tv_sec, tp.tv_nsec);\
}
//resolution >>                           CLOCK_REALTIME: 0, 1
//resolution >>                    CLOCK_REALTIME_COARSE: 0, 1000000
//resolution >>                          CLOCK_MONOTONIC: 0, 1
//resolution >>                   CLOCK_MONOTONIC_COARSE: 0, 1000000
//resolution >>                      CLOCK_MONOTONIC_RAW: 0, 1
//resolution >>                           CLOCK_BOOTTIME: 0, 1
//resolution >>                 CLOCK_PROCESS_CPUTIME_ID: 0, 1
//resolution >>                  CLOCK_THREAD_CPUTIME_ID: 0, 1

int main()
{
    test(CLOCK_REALTIME);
    test(CLOCK_REALTIME_COARSE);
    test(CLOCK_MONOTONIC);
    test(CLOCK_MONOTONIC_COARSE);
    test(CLOCK_MONOTONIC_RAW);
    test(CLOCK_BOOTTIME);
    test(CLOCK_PROCESS_CPUTIME_ID);
    test(CLOCK_THREAD_CPUTIME_ID);
}


