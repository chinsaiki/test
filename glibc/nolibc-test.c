#include <stdio.h>
//#include <unistd.h>
#include "nolibc.h"

#if 1
int main()
{
	my_syscall3(__NR_write, 1, "hello\n", 6);
}
#endif
