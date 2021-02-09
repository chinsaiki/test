#include <unistd.h>

//schedule signal after given number of microseconds
useconds_t ualarm(useconds_t usecs, useconds_t interval);

//注意看ERRORS:EINVAL 说函数中的两个参数不小于1000000时候，将会出错。所以使用时候注意参数的大小。

