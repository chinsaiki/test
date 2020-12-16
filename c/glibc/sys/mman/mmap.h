/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 06月 14日 星期五 09:05:27 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 04月 02日 星期二 08:34:52 CST. */


/**
 *	Example of Memory-Mapped file
 *
 *				address space
 *				of process
 *	high memory +------+
 *              |      |
 *              |      |
 *              +------+--------------------------------\
 *              |      |								|
 *														|
 *														|
 *          memory mapped								|
 *          protion of file								|
 *														|
 *				|      |								|
 *return value->+------+----------------\				|
 *	of mmap  	|      |				|				|
 *	low memory  +------+				|				|
 *										|				|
 *										|				|
 *										|				|
 *										|				|
 *							+------------------------------------+
 *		file referenced by 	|           |memory mapped  |        |
 *				descriptorfd|			|protion of file|		 |
 *							+------------------------------------+
 *							| offset    |  len          |
 */	

/* NAME
 * mmap, munmap - map or unmap files or devices into memory
 *
 * SYNOPSIS*/
#include <sys/mman.h>

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int munmap(void *addr, size_t length);

//prot
//---------------------------------------
//PROT_EXEC  Pages may be executed.
//PROT_READ  Pages may be read.
//PROT_WRITE Pages may be written.
//PROT_NONE  Pages may not be accessed.

//flags:
//---------------------------------------
//MAP_SHARED  Share this mapping. Updates to the mapping are visible to other processes that map this file,
//              and  are  carried  through to the underlying file.  The file may not actually be updated until
//              msync(2) or munmap() is called.
//MAP_PRIVATE Create a private copy-on-write mapping.Updates to the mapping are not visible to other  pro‐
//              cesses  mapping  the  same  file,  and  are not carried through to the underlying file.  It is
//              unspecified whether changes made to the file after the mmap() call are visible in  the  mapped
//              region.
//MAP_32BIT   Put  the  mapping into the first 2 Gigabytes of the process address space.  This flag is supported
//              only on x86-64, for 64-bit programs.  It was added to allow thread stacks to  be  allocated  some‐
//              where in the first 2GB of memory, so as to improve context-switch performance on some early 64-bit
//              processors.  Modern x86-64 processors no longer have this performance problem, so use of this flag
//              is not required on those systems.  The MAP_32BIT flag is ignored when MAP_FIXED is set.
//MAP_ANON    Synonym for MAP_ANONYMOUS.  Deprecated.
//MAP_ANONYMOUS
//         The  mapping  is  not backed by any file; its contents are initialized to zero.  The fd and offset
//         arguments are ignored; however, some implementations require fd to  be  -1  if  MAP_ANONYMOUS  (or
//         MAP_ANON) is specified, and portable applications should ensure this.  The use of MAP_ANONYMOUS in
//         conjunction with MAP_SHARED is supported on Linux only since kernel 2.4.
//
//MAP_DENYWRITE
//         This flag is ignored.  (Long ago, it signaled that attempts to write to the underlying file should
//         fail with ETXTBUSY.  But this was a source of denial-of-service attacks.)
//
//MAP_EXECUTABLE
//        This flag is ignored.
//
//MAP_FILE
//        Compatibility flag.  Ignored.
//
//MAP_FIXED
//         Don't  interpret addr as a hint: place the mapping at exactly that address.  addr must be a multi‐
//         ple of the page size.  If the memory region specified by addr and len overlaps pages of any exist‐
//         ing  mapping(s),  then  the  overlapped part of the existing mapping(s) will be discarded.  If the
//         specified address cannot be used, mmap() will fail.  Because requiring a fixed address for a  map‐
//         ping is less portable, the use of this option is discouraged.
//
//MAP_GROWSDOWN
//         Used  for  stacks.   Indicates  to the kernel virtual memory system that the mapping should extend
//         downward in memory.
//
//MAP_HUGETLB (since Linux 2.6.32)
//         Allocate  the  mapping  using  "huge  pages."   See  the  Linux  kernel  source  file   Documenta‐
//         tion/vm/hugetlbpage.txt for further information.
//
//MAP_LOCKED (since Linux 2.5.37)
//         Lock  the  pages of the mapped region into memory in the manner of mlock(2).  This flag is ignored
//         in older kernels.
//
//MAP_NONBLOCK (since Linux 2.5.46)
//         Only meaningful in conjunction with MAP_POPULATE.  Don't perform read-ahead:  create  page  tables
//         entries  only  for  pages  that  are already present in RAM.  Since Linux 2.6.23, this flag causes
//         MAP_POPULATE to do nothing.  One day the combination of MAP_POPULATE and MAP_NONBLOCK may be reim‐
//         plemented.
//
//MAP_NORESERVE
//         Do  not  reserve  swap space for this mapping.  When swap space is reserved, one has the guarantee
//         that it is possible to modify the mapping.  When swap space is not reserved one might get  SIGSEGV
//         upon  a  write  if  no  physical  memory  is  available.   See  also  the  discussion  of the file
//         /proc/sys/vm/overcommit_memory in proc(5).  In kernels before 2.6, this flag had effect  only  for
//         private writable mappings.
//
//MAP_POPULATE (since Linux 2.5.46)
//         Populate  (prefault) page tables for a mapping.  For a file mapping, this causes read-ahead on the
//         file.  Later accesses to the mapping will not be blocked by page  faults.   MAP_POPULATE  is  sup‐
//         ported for private mappings only since Linux 2.6.23.
//
//MAP_STACK (since Linux 2.6.27)
//         Allocate the mapping at an address suitable for a process or thread stack.  This flag is currently
//         a no-op, but is used in the glibc threading implementation so that if some  architectures  require
//         special treatment for stack allocations, support can later be transparently implemented for glibc.
//
//MAP_UNINITIALIZED (since Linux 2.6.33)
//         Don't  clear  anonymous  pages.  This flag is intended to improve performance on embedded devices.
//         This flag is honored only if the kernel was configured  with  the  CONFIG_MMAP_ALLOW_UNINITIALIZED
//         option.   Because  of  the security implications, that option is normally enabled only on embedded
//         devices (i.e., devices where one has complete control of the contents of user memory).


#if 0
+-------------------------------------------------------+
|                                                       |
|       Kernel 空间
|                                                       |
+-------------------------------------------------------+ 0xC0000000
|                                                       |
|                                                       | 随机偏移
|                                                       |
+-------------------------------------------------------+
|                                                       |
|       栈 向下增长
|                                                       |
+-------------------------------------------------------+
|                                                       |
|                                                       |
|                                                       |
+-------------------------------------------------------+
|                                                       |
|                                                       |
|       内存映射段 (文件映射，匿名映射 /lib/libc.so)
|                                                       |
+-------------------------------------------------------+
|                                                       |
|                                                       |
|                                                       |
+-------------------------------------------------------+ brk();
|                                                       |
|       堆 (向上增长)
|                                                       | brk();开始
+-------------------------------------------------------+ 
|                                                       |
|                                                       | 随机 brk 偏移
|                                                       |
+-------------------------------------------------------+
|                                                       |
|       BSS段                                            |
|       静态变量(0填充，如 static int a;)                       |
|                                                       |
|                                                       |
+-------------------------------------------------------+
|                                                       | end data
|       数据段                                             |
|     被程序员初始化的静态变量(如 static int a = 0;)    |
|                                                       | start data
+-------------------------------------------------------+
|                                                       | end code
|       程序代码区
|       存放函数体的二进制代码
|                                                       | start code
+-------------------------------------------------------+ 0x08048000
|                                                       |
|                                                       |
|                                                       |
|                                                       |
+-------------------------------------------------------+ 0x00000000
    
#endif




/* See NOTES for information on feature test macro requirements. */
