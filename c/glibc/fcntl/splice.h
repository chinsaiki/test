#include <fcntl.h>
#include <unistd.h>

int pipe(int pipefd[2]);
int pipe2(int pipefd[2], int flags);

ssize_t splice(int fd_in, loff_t *off_in, int fd_out, loff_t *off_out, size_t len, unsigned int flags);

/*
sendfile() + DMA Scatter/Gather 的零拷贝方案虽然高效，但是也有两个缺点：
这种方案需要引入新的硬件支持；
虽然 sendfile() 的输出文件描述符在 Linux kernel 2.6.33 版本之后已经可以支持任意类型的文件描述符，但是输入文件描述符依然只能指向文件。
这两个缺点限制了 sendfile() + DMA Scatter/Gather 方案的适用场景。为此，Linux 在 2.6.17 版本引入了一个新的系统调用 splice()，它在功能上和 sendfile() 非常相似，但是能够实现在任意类型的两个文件描述符时之间传输数据；而在底层实现上，splice()又比 sendfile() 少了一次 CPU 拷贝，也就是等同于 sendfile() + DMA Scatter/Gather，完全去除了数据传输过程中的 CPU 拷贝。
splice() 系统调用函数定义如下：
*/
