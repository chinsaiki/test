#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

int foo(int a) {
//    printf("foo a = %d\n", a);
    return a+1;
}
int foo2(int a, int b) {
//    printf("foo2 a = %d, b = %d\n", a, b);
    return foo(a)+foo(b);
}

int main(int argc, char *argv[]) {
    int a = foo2(1, 1);
    return 0;
}
