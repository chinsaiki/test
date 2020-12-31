#include <stdio.h>
#include <unistd.h>

#define MAX 3000

// At one time, this was causing a seg fault within Valgrind -- it was when
// extending the brk segment onto a new page.  Fixed in vg_syscalls.c 1.129.

//[root@localhost tests]# gcc brk2.c 
//[root@localhost tests]# valgrind --quiet ./a.out 
//==128015== Invalid write of size 1
//==128015==    at 0x4005D2: main (in /work/workspace/test/valgrind/valgrind-3.16.1/memcheck/tests/a.out)
//==128015==  Address 0x4224f9f is 999 bytes after the brk data segment limit 0x4224bb8
//==128015== 


int main () {
  char* ptr;
  int i;

  for (i=0; i<MAX; i++) {
    ptr = sbrk(1);//扩展heap的上界，sbrk()返回新的上界地址

    if (ptr == (void*)-1) {
      printf ("sbrk() failed!\n");
      return 0;
    }

    *ptr = 0;
  }
    ptr[1000]='A';
  
  return 0;
} 

