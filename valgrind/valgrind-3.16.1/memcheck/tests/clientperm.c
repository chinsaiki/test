
#include <stdio.h>
#include <stdlib.h>

#include "valgrind/memcheck.h"

//[root@localhost tests]# gcc clientperm.c -g
//[root@localhost tests]# valgrind --quiet ./a.out 
//m_na: returned value is -1
//==128380== Conditional jump or move depends on uninitialised value(s)
//==128380==    at 0x4009E4: main (clientperm.c:29)
//==128380== 
//sum is non-positive
//m_rm: returned value is 1
//==128380== Conditional jump or move depends on uninitialised value(s)
//==128380==    at 0x400AA2: main (clientperm.c:36)
//==128380== 
//sum is non-positive


int main1 ( void )
{
  int xxx, i;
  for (i = 0; i < 10; i++) (void) VALGRIND_CHECK_VALUE_IS_DEFINED(xxx);
  return 0;
}

int main ( void )
{
   int i, sum, m;
   char* aa = calloc(100,1);
   sum = 0;

   (void) VALGRIND_CHECK_MEM_IS_DEFINED(aa,100);

   m = VALGRIND_MAKE_MEM_UNDEFINED( &aa[49], 1 );
   (void) VALGRIND_CHECK_MEM_IS_ADDRESSABLE(aa,100);

   printf("m_na: returned value is %d\n", m );

   for (i = 0; i < 100; i++)
     sum += aa[i];
   printf("sum is %s\n", sum > 0 ? "positive" : "non-positive");

   m = VALGRIND_DISCARD(m);
   printf("m_rm: returned value is %d\n", m );

   for (i = 0; i < 100; i++)
     sum += aa[i];
   printf("sum is %s\n", sum > 0 ? "positive" : "non-positive");

   return 0;
}

