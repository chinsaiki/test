#include <stdio.h>

#define debug() fprintf(stdout, "[%s:%s %d]\n", __FILE__, __func__, __LINE__)

void demo1() {
	debug();
}

void demo2() {
	debug();
	demo1();
}

int main() {
	demo2();
}
