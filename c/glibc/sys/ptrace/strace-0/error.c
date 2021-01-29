
#include <stdlib.h>
#include <stdio.h>
#include "strace.h"

void		exit_error(const char* str)
{
  fprintf(stderr, "Fatal error: %s\n", str);
  exit(1);
}
