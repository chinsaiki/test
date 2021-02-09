
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
//switch process accounting on or off
//acct系统调用可用于启动/禁止进程信息记录功能。
int acct(const char *filename);
//sa -  summarizes accounting information


int main(int argc, char *argv[])
{
	int ret = acct(argv[1]);
	printf("acct: ret = %d, %s\n",ret, strerror(errno));	
	return 0;
}
