#include <stdlib.h>

//[root@localhost tests]# gcc bug340392.c 
//[root@localhost tests]# valgrind --quiet ./a.out 
//==128167== Conditional jump or move depends on uninitialised value(s)
//==128167==    at 0x4005EB: main (in /work/workspace/test/valgrind/valgrind-3.16.1/memcheck/tests/a.out)
//==128167== 


typedef struct {
  unsigned char c;
  int i;
  void *foo;
} S;

S *make_s (void);

int
main (int argc, char **argv)
{
  S *s = make_s ();
  if (s->c == 1 && s->i == 1 && s->foo == getenv ("BLAH"))
    abort();
  return 0;
}

S *
make_s (void)
{
  S *res = malloc (sizeof (S));
  res->c = 1;
  return res;
}

