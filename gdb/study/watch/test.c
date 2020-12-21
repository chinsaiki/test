#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

int demo7()
{
    int a, b, c, d;
    int i;
    for(i=a=b=c=d=0; i<10; i++) {
        a += i;
		if(a % 2 == 0) {
			b += i;
		}
    }
    printf("----------\n");
    return 0;
}
void demo6(){demo7();}
void demo5(){demo6();}
void demo4(){demo5();}
void demo3(){demo4();}
void demo2(){demo3();}
void demo1(){demo2();}
int main(int argc, char *argv[])
{
	demo1();
}
