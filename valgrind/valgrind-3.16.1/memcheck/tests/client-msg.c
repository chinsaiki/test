#include "valgrind/valgrind.h"

//[root@localhost tests]# gcc client-msg.c -g
//[root@localhost tests]# valgrind --quiet ./a.out 
//**128300** hello <> <&>%s world
//==128300==    at 0x4006FC: VALGRIND_PRINTF_BACKTRACE (valgrind.h:6807)
//==128300==    by 0x40073C: baz (client-msg.c:5)
//==128300==    by 0x40074C: bar (client-msg.c:10)
//==128300==    by 0x40075C: foo (client-msg.c:15)
//==128300==    by 0x40076C: main (client-msg.c:20)


void baz()
{
   VALGRIND_PRINTF_BACKTRACE("hello <> %s%%s world\n","<&>");
}

void bar()
{
   baz();
}

void foo()
{
     bar();
}

int main()
{
   foo();

   return 0;
}


