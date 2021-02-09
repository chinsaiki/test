//encrypt, setkey, encrypt_r, setkey_r - encrypt 64-bit messages

#define _XOPEN_SOURCE       /* See feature_test_macros(7) */
#include <unistd.h>

void encrypt(char block[64], int edflag);
//Each of these requires linking with -lcrypt.

//setkey用来设置密钥，而encrypt当然就是来加密（edflag==0）或者解密(edflag==1)的了。
//但是这里有一个需要注意的地方，也就是上面用红色标出的地方，
//setkey的参数key和encrypt的参数block都是64位的位字符，也就
//是说这两个参数是一个长度为64的字符串，而字符串中每一个字符
//的内容不能为别的，只能为'0'或者'1'。


//#define _XOPEN_SOURCE       /* See feature_test_macros(7) */
//#include <stdlib.h>
//
//void setkey(const char *key);
//
//#define _GNU_SOURCE         /* See feature_test_macros(7) */
//#include <crypt.h>
//
//void setkey_r(const char *key, struct crypt_data *data);
//void encrypt_r(char *block, int edflag, struct crypt_data *data);

