

#include <stdio.h>
#include <stdlib.h>
static void* return_arg(void* q);
int main ( void )
{
   void* p = (void*)0x87654321;
   int q[] = { 1, 2, 3 };
   
   /* Free a pointer to Never-Never Land */
   free(p);

   /* Free a pointer to a stack block */
   free(return_arg(q));

   return 0;
}

/*
 * The only purpose of the function below is to make sure that gcc 4.4.x does
 * not print the following warning during the compilation of this test program:
 * warning: attempt to free a non-heap object
 */
static void* return_arg(void* q)
{
   return q;
}

//[root@localhost tests]# valgrind --quiet ./a.out 
//==121306== Invalid free() / delete / delete[] / realloc()
//==121306==    at 0x4C2ACDD: free (vg_replace_malloc.c:530)
//==121306==    by 0x40055E: main (in /work/workspace/test/valgrind/valgrind-3.16.1/memcheck/tests/a.out)
//==121306==  Address 0x87654321 is not stack'd, malloc'd or (recently) free'd
//==121306== 
//==121306== Invalid free() / delete / delete[] / realloc()
//==121306==    at 0x4C2ACDD: free (vg_replace_malloc.c:530)
//==121306==    by 0x400572: main (in /work/workspace/test/valgrind/valgrind-3.16.1/memcheck/tests/a.out)
//==121306==  Address 0xffefffa00 is on thread 1's stack
//==121306==  in frame #1, created by main (???:)
//==121306== 

