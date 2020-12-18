#include "anonyexec.h"
#include "common.h"

//char *argv[] = {"/bin/ls", "-al", "test", NULL};
int anonyexec(const char *path, char *argv[], char *envp[])
{
	int   fd, fdm, filesize;
	void *elfbuf;
	char  cmdline[256];
	
	fd = open(path, O_RDONLY);
	IF(fd < 0, -1);
	filesize = lseek(fd, SEEK_SET, SEEK_END);
	lseek(fd, SEEK_SET, SEEK_SET);
	IF(filesize <= 0, -1);
	elfbuf = malloc(filesize);
	read(fd, elfbuf, filesize);
	close(fd);

    //memfd_create - create an anonymous file 创建一个匿名文件
    //int memfd_create(const char *name, unsigned int flags);
	fdm = syscall(__NR_memfd_create, "elf", MFD_CLOEXEC); //创建一个匿名文件
	GO(fdm < 0, __clean);
	GO(0 != ftruncate(fdm, filesize), __clean);

    /* 把这个可执行文件 写入 匿名 文件 描述符中 */
	write(fdm, elfbuf, filesize);
	free(elfbuf);
    
	sprintf(cmdline, "/proc/self/fd/%d", fdm);
    
	argv[0] = cmdline;
	execve(argv[0], argv, envp);

    debug("execve return.\r\n");
    
	return 0;
__clean:
	free(elfbuf);
	return -1;
}
