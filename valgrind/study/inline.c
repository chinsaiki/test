#include <malloc.h>
#include <stdio.h>

#ifdef INLINE
#define _inline inline
#else
#define _inline
#endif

#define F(n) \
	_inline void func##n()

F(1);F(2);F(3);F(4);F(5);

_inline void func1(){func2();}
_inline void func2(){func3();}
_inline void func3(){func4();}
_inline void func4(){func5();}
_inline void func5(){
	printf("Call %s:%d\n", __func__, __LINE__);
}

int main()
{
	func1();
    printf("Exit program.\n");
}
