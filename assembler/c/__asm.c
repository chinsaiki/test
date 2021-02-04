#include <stdio.h>

int main() {

	char a[10] = {"abcd"};

	__asm
	{
	push ebx
	push eax
	push ecx
	}

	printf("%s\n", a);
	
	return 0;
}
