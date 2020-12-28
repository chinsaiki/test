#include <stdio.h>

/**
 *	gcc -m32 hello.c	输出为 4
 *	gcc -m64 hello.c	输出为 8
 */

int main()
{
	printf("sizeof unsigned long  == %d\n", sizeof(unsigned long));
}
