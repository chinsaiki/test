#include "elfreader.h"
#include "common.h"

uint64_t elfentry(const char *path)
{
	Elf64_Ehdr ehdr;
	int fd;
	
	fd = open(path, O_RDONLY);
	IF(fd < 0, 0);
	read(fd, (void*)&ehdr, sizeof(ehdr));
	close(fd);
    /* 虚拟地址起点 */
	return ehdr.e_entry;
}
