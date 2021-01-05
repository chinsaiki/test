#include <math.h>
#include <stdio.h>

static int count  = 0;
int __count = 0;

static void __attribute__((constructor(101))) __init_count(){
	static int __cnt = 1024;
	count=__cnt*2;
	__count = __cnt*4;
	__cnt *= 2;
}

static void __attribute__((constructor(101))) __init_sleep() {
	printf("ready to running..\n");
	sleep(1);
}

int add(int a, int b)
{
	count ++;
	__count ++;

	printf("count = %d, %d\n", count, __count);
	return a+b;
}
