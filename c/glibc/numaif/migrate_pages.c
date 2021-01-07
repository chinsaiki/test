#include <stdio.h>
#include <numa.h>
#include <numaif.h>
#include <errno.h>


//gcc migrate_pages.c -lnuma

//这段代码将在 numa 系统上运行成功，而在我的 虚拟机上（只有一个内存节点）上失败
//    [root@localhost rt]# ./a.out 
//    numa_max_node = 1
//    migrate_pages = 0
//    migrate_pages: : Success
//    EPERM = 1
//    ESRCH = 3
//
//    [rongtao@localhost numaif]$ gcc migrate_pages.c -lnuma
//    [rongtao@localhost numaif]$ ./a.out 
//    numa_max_node = 0
//    migrate_pages = -1
//    migrate_pages: : Invalid argument
//    EPERM = 1
//    ESRCH = 3

int main()
{
    unsigned long old_nodes = 0;
    unsigned long new_nodes = 1;
    

    char *str = numa_alloc(numa_pagesize());
    printf("numa_max_node = %d\n", numa_max_node());

    long ret = migrate_pages(getpid(), numa_max_node(), &old_nodes, &new_nodes);
    printf("migrate_pages = %ld\n", ret);
    perror("migrate_pages: ");
    printf("EPERM = %ld\n", EPERM);
    printf("ESRCH = %ld\n", ESRCH);
    

    numa_free(str, numa_pagesize());
}



