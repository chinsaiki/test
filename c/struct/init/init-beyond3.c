#include <stdio.h>

#define N 3

//好牛逼的操作
struct e_t {
	int id;
}entity[N] = {
	[0 ... N-1] = {id:10},
};

struct e_t e1[N] = {
	[0 ... N-1] = {id:10},
};

struct e_t e2[N] = {
	[0 ... N-1] = {.id = 10},
};

struct e_t e3[N] = {
	[0 ... 1] = {id:10},
	[2 ... N-1] = {.id = 11}
};

int main()
{
	int i;
	for(i=0;i<N;i++)
	{
		printf("ENTITY%d, %d\n", i, entity[i].id);
		printf("E1    %d, %d\n", i, e1[i].id);
		printf("E2    %d, %d\n", i, e2[i].id);
		printf("E3    %d, %d\n", i, e3[i].id);
	}

	return 0;
}
