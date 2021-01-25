#include "slabinfo.h"

#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <time.h>

static const unsigned DELAY = 10;

void skip_first_lines(FILE* slabinfo_procfile) {
    fscanf(slabinfo_procfile, "slabinfo - version: 2.1\n# name            <active_objs> <num_objs> <objsize> <objperslab> <pagesperslab> : tunables <limit> <batchcount> <sharedfactor> : slabdata <active_slabs> <num_slabs> <sharedavail>");
}

void read_slabinfo_line(FILE* slabinfo_procfile, struct slabinfo* slab) {
    fscanf(slabinfo_procfile, "%20s%u%u%u%u%u : tunables%u%u%u : slabdata%u%u%u\n", slab->name, &(slab->active_obj), &(slab->num_obj),
           &(slab->objsize), &(slab->objperslab), &(slab->pagesperslab), &(slab->limit), &(slab->batchcount), &(slab->sharedfactor),
           &(slab->active_slabs), &(slab->num_slabs), &(slab->sharedavail));
    slab->name[19] = '\0';
}

void display_kmalloc_usage(FILE* log_stream) {
    unsigned allocated_memory = 0;
    unsigned used_memory = 0;

    for (unsigned i = 0; i < SLABINFO_LINES; ++i) {
        if (strstr(slabinfo_array[i].name, "kmalloc") != NULL) {
            allocated_memory += slabinfo_array[i].num_obj * slabinfo_array[i].objsize;
            used_memory += slabinfo_array[i].active_obj * slabinfo_array[i].objsize;
        }
    }
    time_t rawtime;
    time(&rawtime);

    struct tm* timeinfo = localtime(&rawtime);

    fprintf(log_stream, "%s\tActive:\t\t%.1f kB\n\tAllocated:\t%.1f kB\n\tUsage:\t\t%2.1f %%\n",
            asctime(timeinfo), used_memory / 1024.0, allocated_memory / 1024.0, used_memory * 100.0 / allocated_memory);
}

void slabinfo_cycle(FILE* log_stream) {
    FILE* slabinfo_procfile = fopen("/proc/slabinfo", "r");
    if (slabinfo_procfile == NULL) {
        return;
    }
    skip_first_lines(slabinfo_procfile);
    for (size_t i = 0; i < SLABINFO_LINES; ++i) {
        read_slabinfo_line(slabinfo_procfile, slabinfo_array + i);
    }

    display_kmalloc_usage(log_stream);

    sleep(DELAY);
}
