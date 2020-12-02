
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
	char *name[4];
	int age;
}people;

/*	简单的映射一个文件，并以people数据结构的格式从mmap()返回的地址处读取10个people
 *	数据结构，并输出读取的值，然后解除映射。
 */
#define debug() printf("%s:%d\n", __func__, __LINE__)

int main(int argc, char *argv[])
{
	int fd, i, ret;
	people *p_map;
	debug();
	/**
	 *	argv[1] maybe is /dev/zero
	 */
	fd = open("/dev/zero",O_CREAT | O_RDWR, 00777);
	debug();
    
	//将文件映射之内存中
	p_map = (people*)mmap(NULL, sizeof(people)*1024, 
				PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	debug();

    /* 建议使用大页内存 */
    ret = madvise(p_map, sizeof(people)*1024, MADV_HUGEPAGE);
    perror("madvise");

	if(p_map == (void *)-1)
	{
		fprintf(stderr, "mmap: %s\n", strerror(errno));
		return ;
	}
	debug();
	for(i=0;i<10; i++)
	{
		debug();
		printf("name: %s age:%d\n", (*(p_map+i)).name, (*(p_map+i)).age);
		debug();
	}
	debug();
	munmap(p_map, sizeof(people)*10);

	return 0;
}

