#include <stdio.h>
#include <numa.h>


//gcc numa_available.c -lnuma


int main()
{
    printf("numa_preferred = %d\n", numa_preferred());
}


