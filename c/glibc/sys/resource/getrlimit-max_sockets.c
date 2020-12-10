
#include <sys/resource.h>
#include <stdio.h>
#include <unistd.h>

// struct rlimit{
    // rlim_t  rlim_cur;
    // rlim_t  rlim_max;
// };

int main()
{
    int max_sockets;
    struct rlimit _rlimit, rlimit2, rlimit3;
    
    getrlimit(RLIMIT_NOFILE, &rlimit3);

    max_sockets =  rlimit3.rlim_cur;
    //最大的socket fd 数量
    printf("max_sockets =  rlimit3.rlim_cur = %d\n", max_sockets);
	return 0;
}
