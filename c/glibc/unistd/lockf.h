//lockf - apply, test or remove a POSIX lock on an open file

#include <unistd.h>

int lockf(int fd, int cmd, off_t len);


/*系统调用lockf（files,function,size),
用作锁定文件的某些段或者整个文件。本函数的头文件为include<unistd.h>
其中file为文件描述符，function是锁定和解锁：1表示锁定，0表示解锁；
size是锁定或者解锁的字节数，为0，表示从文件的当前位置到文件尾。*/

