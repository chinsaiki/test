#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main()
{
	int fd = open("b.txt.out", O_RDWR|O_CREAT|O_TRUNC, S_IWUSR|S_IWGRP|S_IWOTH);
	
	printf("open = %d\n", fd);

	//关闭了 标准输出
	close(STDOUT_FILENO);

	//复制 打开文件的 fd， 这里会生成 fd = 1 （标准输出）
    fprintf(stderr, "dup = %d\n", dup(fd));

	//再往 fd = 1 写入时， 将同步到 打开的 文件描述符中
    printf("abcd pid = %d\n", getpid());
    close(fd);

	return 1;
}
