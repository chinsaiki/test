#include <stdio.h>
#include <numa.h>


//gcc numa_available.c -lnuma


int main()
{
    printf("numa_max_possible_node = %d\n", numa_max_possible_node());
}


