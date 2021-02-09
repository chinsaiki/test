
#define _XOPEN_SOURCE       /* See feature_test_macros(7) */
#include <unistd.h>

//swap adjacent bytes 交换相邻字节
void swab(const void *from, void *to, ssize_t n);

