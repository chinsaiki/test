#include <linux/getcpu.h>

//getcpu - determine CPU and NUMA node on which the calling thread is running
int getcpu(unsigned *cpu, unsigned *node, struct getcpu_cache *tcache);

//Note: There is no glibc wrapper for this system call; see NOTES.
