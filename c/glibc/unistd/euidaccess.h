#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <unistd.h>

//euidaccess, eaccess - check effective user's permissions for a file
int euidaccess(const char *pathname, int mode);
int eaccess(const char *pathname, int mode);

//我们从unistd文件中的注解可以看到区别。access是通过实际UID，GID进行权限测试，
//    而eaccess(euidaccess)是和open函数一样，通过有效UID，GID进行权限测试。
//    而faccessat可以通过flag参数设置AT_EACESS去改变，是用有效还是实际。

