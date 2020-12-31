#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>

//[root@localhost tests]# gcc buflen_check.c 
//[root@localhost tests]# valgrind --quiet ./a.out 
//==128068== Syscall param socketcall.getsockname(name) points to unaddressable byte(s)
//==128068==    at 0x4F35687: getsockname (in /usr/lib64/libc-2.17.so)
//==128068==    by 0x4006B3: main (in /work/workspace/test/valgrind/valgrind-3.16.1/memcheck/tests/a.out)
//==128068==  Address 0x0 is not stack'd, malloc'd or (recently) free'd
//==128068== 
//==128068== Syscall param socketcall.getsockname(namelen_in) points to unaddressable byte(s)
//==128068==    at 0x4F35687: getsockname (in /usr/lib64/libc-2.17.so)
//==128068==    by 0x4006CC: main (in /work/workspace/test/valgrind/valgrind-3.16.1/memcheck/tests/a.out)
//==128068==  Address 0x0 is not stack'd, malloc'd or (recently) free'd
//==128068== 
//getsockname(1) failed
//getsockname(2) failed


int main(void)
{
   struct sockaddr name;
   int res1, res2, res3;
   unsigned len = 10;

   res1 = socket(PF_UNIX, SOCK_STREAM, 0);
   if (res1 == 0) {
      fprintf(stderr, "socket() failed\n");
      exit(1);
   }

   /* Valgrind 1.0.X doesn't report the second error */
   res2 = getsockname(res1, NULL,  &len);    /* NULL is bogus */
   res3 = getsockname(res1, &name, NULL);    /* NULL is bogus */
   if (res2 == -1) {
      fprintf(stderr, "getsockname(1) failed\n");
   }
   if (res3 == -1) {
      fprintf(stderr, "getsockname(2) failed\n");
   }
   
   return 0;
}


