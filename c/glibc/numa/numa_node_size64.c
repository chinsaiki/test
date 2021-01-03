#include <stdio.h>
#include <numa.h>


//gcc numa_available.c -lnuma


int main()
{
    long long ret;
    long long freep = 0;
    
    ret = numa_node_size64(0, &freep);
    printf("numa_node_size64 : ret = %ld G, freep = %ld G\n", ret/1024/1024, freep/1024/1024);
}



