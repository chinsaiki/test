#include <stdio.h>

//编译不通过,后面再看
int main()
{
	int a = 100, b = 200, c;

	asm ("add %0,%1,%2\n\t"
		: "r="(c)
		: "r"(a),"r"(b)
		: "memory"
	);

	printf("c = %d\n", c);
}
