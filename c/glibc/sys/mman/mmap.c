/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 06月 14日 星期五 09:05:27 CST. */
/* 
 Copyright (C) Rong Tao @Beijing

 Permission is granted to copy, distribute and/or modify this document
 under the terms of the GNU Free Documentation License, Version 1.3
 or any later version published by the Free Software Foundation;
 with no Invariant Sections, no Front-Cover Texts, and no Back-Cover
 Texts. A copy of the license is included in the section entitled ‘‘GNU
 Free Documentation License’’.
   2019年 03月 14日 星期四 19:24:31 CST. 
*/
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 13日 星期三 08:54:03 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 12日 星期二 15:07:31 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 08日 星期五 08:10:09 CST. */
/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 07日 星期四 20:27:47 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 07日 星期四 20:26:54 CST. */
/*	内存映射
 *
 *	void * mmap (void *address, size t length,int protect, int flags, int [Function]
 *	filedes, oﬀ t offset)
 *	The mmap function creates a new mapping, connected to bytes (oﬀset) to (oﬀset +
 *	length - 1) in the fle open on fledes. A new reference for the fle specifed by fledes
 *	is created, which is not removed by closing the fle.
 *	address gives a preferred starting address for the mapping. NULL expresses no prefer
 *	ence. Any previous mapping at that address is automatically removed. The address
 *	you give may still be changed, unless you use the MAP_FIXED ﬂag.
 *	protect contains ﬂags that control what kind of access is permitted. They include
 *	PROT_READ, PROT_WRITE, and PROT_EXEC, which permit reading, writing, and exe
 *	cution, respectively. Inappropriate access will cause a segfault (see Section 24.2.1
 *	[Program Error Signals], page 604).
 *	Note that most hardware designs cannot support write permission without read per
 *	mission, and many do not distinguish read and execute permission. Thus, you may
 *	receive wider permissions than you ask for, and mappings of write-only fles may be
 *	denied even if you do not use PROT_READ.
 *	ﬂags contains ﬂags that control the nature of the map. One of MAP_SHARED or MAP_
 *	PRIVATE must be specifed.
 */
    /**
     *  Example of Memory-Mapped file
     *
     *              address space
     *              of process
     *  high memory +------+
     *              |      |
     *              |      |
     *              +------+--------------------------------\
     *              |      |                                |
     *                                                      |
     *                                                      |
     *          memory mapped                               |
     *          protion of file                             |
     *                                                      |
     *              |      |                                |
     *return value->+------+----------------\               |
     *  of mmap     |      |                |               |
     *  low memory  +------+                |               |
     *                                      |               |
     *                                      |               |
     *                                      |               |
     *                                      |               |
     *                          +------------------------------------+
     *      file referenced by  |           |memory mapped  |        |
     *              descriptorfd|           |protion of file|        |
     *                          +------------------------------------+
     *                          | offset    |  len          |
     */ 

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
	int fd, i;
	people *p_map;
	debug();
	/**
	 *	argv[1] maybe is /dev/zero
	 */
	fd = open(argv[1],O_CREAT | O_RDWR, 00777);
	debug();
	//将文件映射之内存中
	p_map = (people*)mmap(NULL, sizeof(people)*10, 
				PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	debug();
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
