#include <stdio.h>
#include "atomic.h"

int main()
{
    unsigned long int val1, val2;

    val1 = val2 = 0;
    
    val2 = atomic_exchange_acq(&val1, 1);
    printf("val1 = %x, val2 = %x.\n", val1, val2);
    
    val2 = atomic_exchange_acq(&val1, 0x0ff1);
    printf("val1 = %x, val2 = %x.\n", val1, val2);

    atomic_and(&val1, 0xff01);
    printf("val1 = %x, val2 = %x.\n", val1, val2);
}
