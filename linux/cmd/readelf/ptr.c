#include <stdio.h>

typedef void (*foo_t)();

struct test_struct {
	foo_t f;
};

void foo()
{
	printf("foo.\n");
}

int main()
{
	struct test_struct test[2];
	test[0].f = foo;
	test[1].f = foo;

	test[0].f();
	test[1].f();
}
