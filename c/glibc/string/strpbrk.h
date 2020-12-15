#include <string.h>

//search a string for any of a set of bytes
char *strpbrk(const char *s, const char *accept);



static void demo()
{
	char str[] = {"n"}, s[256] = {"No.s rongtao "};

	char *ret = strpbrk(s, str);
	//返回值为s中搜索str中所有的字符，当返回第一个指针地址:


	printf("%s\n%s\n%s\n",ret, str,s);
	//ret o.s rongtao , rong:No.s rongtao
}

