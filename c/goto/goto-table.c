/**
 *	跳转表
 *	2021年2月19日 rtoax
 */
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	static void *jump_table[3] = {
		&&label1, &&label2, && label3
	};

	int jmp = argv[1]?atoi(argv[1]):0;

	goto *jump_table[jmp];

label1:
	printf("label1.\n");
label2:
	printf("label2.\n");
label3:
	printf("label3.\n");
}
