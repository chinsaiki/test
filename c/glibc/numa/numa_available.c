#include <stdio.h>
#include <numa.h>


//gcc numa_available.c -lnuma


int main()
{
    printf("numa_available = %d\n", numa_available());
}
