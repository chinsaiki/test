#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


int main(int argc, char *argv[])
{
	char *buf;
	char filename[64] = "";
	struct stat stat;
	int size = 100 * 4096;
	int fd = 0;

	strcpy(filename, argv[1]);

	fd = open(filename, O_RDWR|O_CREAT, 0664);
	fstat(fd, &stat);

	buf = mmap(NULL, stat.st_size, PROT_WRITE, MAP_PRIVATE, fd, 0);
	memset(buf, 0x55, stat.st_size);

	madvise(buf, stat.st_size, MADV_MERGEABLE);

	while(1) {
		sleep(1);
	}
}

#if 0
Kernel SamePage Merging - 同页合并

-----------
1. 编译源代码
	gcc test.c 

2. 使用 dd 命令创建 ksm.dat.out 文件 , 大小 100M
	dd if=/dev/zero of=ksm.dat.out bs=1M count=100

3. 使能 KSM
	echo 1 > /sys/kernel/mm/ksm/run

4. 运行 test.c 程序
	./a.out ksm.dat.out

5. 查看系统有多少页被 合并 了
	cat /sys/kernel/mm/ksm/pages_sharing
	cat /sys/kernel/mm/ksm/pages_shared
	cat /sys/kernel/mm/ksm/pages_unshared
#endif
