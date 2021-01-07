#include <numaif.h>

//migrate_pages - move all pages in a process to another set of nodes
long migrate_pages(int pid, unsigned long maxnode,
                    const unsigned long *old_nodes,
                    const unsigned long *new_nodes);


SYSCALL_DEFINE4(migrate_pages, pid_t, pid, unsigned long, maxnode,
                    const unsigned long __user *, old_nodes,
                    const unsigned long __user *, new_nodes)
{
    return kernel_migrate_pages(pid, maxnode, old_nodes, new_nodes);
}

