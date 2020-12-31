
#include <stdio.h>

//[root@localhost tests]# gcc badloop.c 
//[root@localhost tests]# valgrind --quiet ./a.out 
//==127808== Conditional jump or move depends on uninitialised value(s)
//==127808==    at 0x40057B: main (in /work/workspace/test/valgrind/valgrind-3.16.1/memcheck/tests/a.out)
//==127808== 


int main ( void )
{
   int a[5];
   int i, s;
   a[0] = a[1] = a[3] = a[4] = 0;
   s = 0;
   for (i = 0; i < 5; i++) 
      s += a[i];
   if (s == 377)
      printf("sum is %d\n", s);
   return 0;
}

