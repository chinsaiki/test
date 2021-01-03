#include <stdio.h>
#include <numa.h>


//gcc numa_available.c -lnuma


int main()
{
    long  ret;
    long  freep = 0;
    
    ret = numa_node_size(0, &freep);
    printf("numa_node_size : ret = %ld G, freep = %ld G\n", ret/1024/1024, freep/1024/1024);
}




