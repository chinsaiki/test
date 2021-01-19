#include <sys/eventfd.h>

//如果计数器中的值大于 0：
//    * 若设置了 EFD_SEMAPHORE ，则返回 1，且计数器中的值 -1；
//    * 若未设置 EFD_SEMAPHORE ，则返回 计数器中的值，且计数器中的值 置 0；
//    
//如果计数器中的值等于 0：
//    * 如果设置了 EFD_NONBLOCK 直接返回 -1；
//    * 如果未设置 EFD_NONBLOCK 则会一直阻塞，直到计数器中的值 大于 0；

/* Read event counter and possibly wait for events.  */
extern int eventfd_read (int __fd, eventfd_t *__value);

