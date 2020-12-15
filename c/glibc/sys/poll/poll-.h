#include <poll.h>
/**
 *	poll函数起源于SVR3，最初局限于流设备，SVR3取消了这种限制，允许poll工作在任何
 *	描述符上。poll提供的功能与select类似，不过在处理流设备时，它能够提供额外的信息。
 *
 *	第一个参数值指向一个机构数组第一个元素的指针。每个数组元素都是一个pollfd结构，
 *	用于制定测试某个给定描述符fd的条件。
 *		
 *		struct pollfd{
 *			int fd;			//用于检查的描述符
 *			short events;	//fd上的事件
 *			short revents;	//fd上发生的事件
 *		}
 */


/* Type used for the number of file descriptors.  */
typedef unsigned long int nfds_t;

/* Data structure describing a polling request.  */
struct pollfd {
   int   fd;         /* file descriptor */
   short events;     /* requested events */
   short revents;    /* returned events */
};

//wait for some event on a file descriptor
int poll(struct pollfd *fds, nfds_t nfds, int timeout);


/** 百度百科 **
 *
 *	poll是linux中的字符设备驱动中的一个函数，linux2.5.44版本后，poll被epoll取代。
 *	和select实现的功能差不多，poll的作用是把当前的文件指针挂到等待队列。
 *
 *	作用：把当前的文件指针挂到等待队列；
 *	功能：和select差不多；
 *
 */
/**
  POLLIN        普通或优先级带数据可读
  POLLRDNORM    普通数据可读
  POLLRDBAND    优先级带数据可读
  POLLPRI       高优先级数据可读
  
  POLLOUT       普通数据可写
  POLLWRNORM    普通数据可写
  POLLWRBAND    优先级带数据可写
  
  POLLERR       发生错误
  POLLHUP       发生挂起
  POLLNVAL      描述符不是一个可打开的文件

 */

