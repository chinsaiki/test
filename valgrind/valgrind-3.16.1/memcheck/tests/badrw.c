#include <stdlib.h>

//[root@localhost tests]# gcc badrw.c 
//[root@localhost tests]# valgrind --quiet ./a.out 
//==127910== Invalid read of size 2
//==127910==    at 0x40057F: main (in /work/workspace/test/valgrind/valgrind-3.16.1/memcheck/tests/a.out)
//==127910==  Address 0x520403c is 4 bytes before a block of size 10 alloc'd
//==127910==    at 0x4C29BE3: malloc (vg_replace_malloc.c:299)
//==127910==    by 0x40053E: main (in /work/workspace/test/valgrind/valgrind-3.16.1/memcheck/tests/a.out)
//==127910== 
//==127910== Invalid write of size 2
//==127910==    at 0x40058E: main (in /work/workspace/test/valgrind/valgrind-3.16.1/memcheck/tests/a.out)
//==127910==  Address 0x520403c is 4 bytes before a block of size 10 alloc'd
//==127910==    at 0x4C29BE3: malloc (vg_replace_malloc.c:299)
//==127910==    by 0x40053E: main (in /work/workspace/test/valgrind/valgrind-3.16.1/memcheck/tests/a.out)
//==127910== 
//==127910== Invalid read of size 1
//==127910==    at 0x400595: main (in /work/workspace/test/valgrind/valgrind-3.16.1/memcheck/tests/a.out)
//==127910==  Address 0x520403f is 1 bytes before a block of size 10 alloc'd
//==127910==    at 0x4C29BE3: malloc (vg_replace_malloc.c:299)
//==127910==    by 0x40053E: main (in /work/workspace/test/valgrind/valgrind-3.16.1/memcheck/tests/a.out)
//==127910== 
//==127910== Invalid write of size 1
//==127910==    at 0x4005A3: main (in /work/workspace/test/valgrind/valgrind-3.16.1/memcheck/tests/a.out)
//==127910==  Address 0x520403f is 1 bytes before a block of size 10 alloc'd
//==127910==    at 0x4C29BE3: malloc (vg_replace_malloc.c:299)
//==127910==    by 0x40053E: main (in /work/workspace/test/valgrind/valgrind-3.16.1/memcheck/tests/a.out)
//==127910== 


int main(void)
{
   void* x = malloc(10);

   int   *x4;
   short *x2;
   char  *x1;
   int    y4;
   short  y2;
   char   y1;

   x4 = x-4000; //这种超出检测区，检测不到
   x2 = x-4;
   x1 = x-1;

   // Invalid reads and writes of sizes 4, 2, 1
   y4 = *x4;
   *x4 = y4;

   y2 = *x2;
   *x2 = y2;

   y1 = *x1;
   *x1 = y1;
   
   return 0;
}

