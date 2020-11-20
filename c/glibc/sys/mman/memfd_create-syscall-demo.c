#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/memfd.h>
#include <sys/syscall.h>
#include <errno.h>
 
int anonyexec(const char *path, char *argv[])
{
    int   fd, fdm, filesize;
    void *elfbuf;
    char  cmdline[256];

    //打开可执行文件 ELF
    fd = open(path, O_RDONLY);
    filesize = lseek(fd, SEEK_SET, SEEK_END);
    lseek(fd, SEEK_SET, SEEK_SET);
    elfbuf = malloc(filesize);
    read(fd, elfbuf, filesize);
    close(fd);

    //创建对应的 memory fd
    fdm = syscall(__NR_memfd_create, "elf", MFD_CLOEXEC);
    ftruncate(fdm, filesize);
    write(fdm, elfbuf, filesize);
    free(elfbuf);

    //执行内存中的代码
    sprintf(cmdline, "/proc/self/fd/%d", fdm);
    argv[0] = cmdline;
    execve(argv[0], argv, NULL);
    free(elfbuf);
    return -1;
}
 
int main()
{
    char *argv[] = {"/bin/uname", "-a", NULL};
    int result =anonyexec("/bin/uname", argv);
    return result;
}

