#include <stdio.h>
#include <numa.h>


//gcc numa_available.c -lnuma


int main()
{
    printf("numa_pagesize = %d\n", numa_pagesize());
}


