#pragma once

#include <stdio.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>


static struct cpu_freq {
    enum {CPU_3GMHZ, CPU_2_7GMHZ,}index;
    unsigned long freq;
    char *string;
}CPU_MHZ[] = {
    {CPU_3GMHZ, 3000000000, "3GMHz"},
    {CPU_2_7GMHZ, 2700000000, "2.7GMHz"},
};
    

static unsigned long __get_ticks(void)
{
    unsigned long ret;
    union
    {
        unsigned long tsc_64;
        struct
        {
            unsigned int lo_32;
            unsigned int hi_32;
        };
    } tsc;

    __asm volatile("rdtsc" :
             "=a" (tsc.lo_32),
             "=d" (tsc.hi_32));

     ret = ((unsigned long)tsc.tsc_64);
     return ret;
}

static inline void set_msg_timestamp(unsigned long *timestamp)
{
    *timestamp = __get_ticks();
}
static inline unsigned long call_timestamp_diff(unsigned long *timestamp)
{
    return __get_ticks() - *timestamp;
}

static unsigned long int diff_timeval_usec(struct timeval *big, struct timeval *small)
{
    unsigned long int diffsec = big->tv_sec - small->tv_sec;
    unsigned long int diffusec = diffsec*1000000 + (big->tv_usec - small->tv_usec);
//    printf("%ld MicroSec\n", diffusec);
    return diffusec;
}

