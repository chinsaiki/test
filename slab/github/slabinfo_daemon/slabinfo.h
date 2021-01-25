#ifndef SLABINFO_H
#define SLABINFO_H

#define SLABINFO_LINES 151

#include <stdio.h>
#include <unistd.h>

struct slabinfo {
    char name[20];

    unsigned active_obj;
    unsigned num_obj;
    unsigned objsize;
    unsigned objperslab;
    unsigned pagesperslab;

    unsigned limit;
    unsigned batchcount;
    unsigned sharedfactor;

    unsigned active_slabs;
    unsigned num_slabs;
    unsigned sharedavail;
};

static struct slabinfo slabinfo_array[SLABINFO_LINES];

void slabinfo_cycle(FILE* log_stream);

#endif // SLABINFO_H
