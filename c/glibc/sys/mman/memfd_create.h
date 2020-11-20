#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <sys/mman.h>

//memfd_create - create an anonymous file 创建一个匿名文件
int memfd_create(const char *name, unsigned int flags);

