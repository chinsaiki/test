#include <sys/eventfd.h>

//如果写入值的和小于 0xFFFFFFFFFFFFFFFE ,则写入成功；
//如果写入值的和大于 0xFFFFFFFFFFFFFFFE ,
//    * 如果设置了 EFD_NONBLOCK 直接返回 -1；
//    * 如果未设置 EFD_NONBLOCK 则会移植阻塞，直到read操作进行；


/* Increment event counter.  */
extern int eventfd_write (int __fd, eventfd_t __value);

