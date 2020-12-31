#include <assert.h>
#include <stdlib.h>
#include <sys/poll.h>

// At one point, poll()'s checking was not done accurately.  This test
// exposes this -- previously Memcheck only found one error, now if finds
// two.

//[root@localhost tests]# gcc badpoll.c 
//[root@localhost tests]# valgrind --quiet ./a.out 
//==127858== Syscall param poll(ufds.fd) points to uninitialised byte(s)
//==127858==    at 0x4F29C20: __poll_nocancel (in /usr/lib64/libc-2.17.so)
//==127858==    by 0x40063A: main (in /work/workspace/test/valgrind/valgrind-3.16.1/memcheck/tests/a.out)
//==127858==  Address 0x5204048 is 8 bytes inside a block of size 15 alloc'd
//==127858==    at 0x4C29BE3: malloc (vg_replace_malloc.c:299)
//==127858==    by 0x4005DE: main (in /work/workspace/test/valgrind/valgrind-3.16.1/memcheck/tests/a.out)
//==127858== 
//==127858== Syscall param poll(ufds.revents) points to unaddressable byte(s)
//==127858==    at 0x4F29C20: __poll_nocancel (in /usr/lib64/libc-2.17.so)
//==127858==    by 0x40063A: main (in /work/workspace/test/valgrind/valgrind-3.16.1/memcheck/tests/a.out)
//==127858==  Address 0x520404f is 0 bytes after a block of size 15 alloc'd
//==127858==    at 0x4C29BE3: malloc (vg_replace_malloc.c:299)
//==127858==    by 0x4005DE: main (in /work/workspace/test/valgrind/valgrind-3.16.1/memcheck/tests/a.out)
//==127858== 


int main(void)
{
   // Under-allocate by one byte so we get an addressability error.
   struct pollfd* ufds = malloc(2 * sizeof(struct pollfd) - 1);
   assert(ufds);

   ufds[0].fd = 0;
   ufds[0].events = 0;
//   ufds[1].fd = 0;    // leave undefined so we get another error.
   ufds[1].events = 0;

   // Previously, the bounds-error due to the under-allocation was detected,
   // but not the undefined value error due to ufds[1].fd not being defined.
   poll(ufds, 2, 200);

   return 0;
}

