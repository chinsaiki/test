
#include <stdio.h>
#include <stdlib.h>

int main ( void )
{
   char* aa = malloc(8);
   aa[-1] = 17;
   if (aa[-1] == 17) 
      printf("17\n"); else printf("not 17\n");
   return 0;
}
//[root@localhost tests]# valgrind --quiet ./a.out 
//==121283== Invalid write of size 1
//==121283==    at 0x40059B: main (in /work/workspace/test/valgrind/valgrind-3.16.1/memcheck/tests/a.out)
//==121283==  Address 0x520403f is 1 bytes before a block of size 8 alloc'd
//==121283==    at 0x4C29BE3: malloc (vg_replace_malloc.c:299)
//==121283==    by 0x40058E: main (in /work/workspace/test/valgrind/valgrind-3.16.1/memcheck/tests/a.out)
//==121283== 
//==121283== Invalid read of size 1
//==121283==    at 0x4005A6: main (in /work/workspace/test/valgrind/valgrind-3.16.1/memcheck/tests/a.out)
//==121283==  Address 0x520403f is 1 bytes before a block of size 8 alloc'd
//==121283==    at 0x4C29BE3: malloc (vg_replace_malloc.c:299)
//==121283==    by 0x40058E: main (in /work/workspace/test/valgrind/valgrind-3.16.1/memcheck/tests/a.out)
//==121283== 

