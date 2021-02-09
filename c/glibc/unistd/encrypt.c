#define _XOPEN_SOURCE
#include <unistd.h>
#include <stdio.h>

//gcc encrypt.c -lcrypt
//./a.out 
//
//setkey用来设置密钥，而encrypt当然就是来加密（edflag==0）或者解密(edflag==1)的了。
//但是这里有一个需要注意的地方，也就是上面用红色标出的地方，
//setkey的参数key和encrypt的参数block都是64位的位字符，也就
//是说这两个参数是一个长度为64的字符串，而字符串中每一个字符
//的内容不能为别的，只能为'0'或者'1'。

int main(void)
{
    char key[64];      /* bit pattern for key */
    char txt[64];      /* bit pattern for messages */

    setkey(key);
    encrypt(txt, 0);   /* encode */
    encrypt(txt, 1);   /* decode */
}

