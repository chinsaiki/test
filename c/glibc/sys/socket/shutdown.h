/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 06月 14日 星期五 09:05:29 CST. */
/* 
 Copyright (C) Rong Tao @Beijing

 Permission is granted to copy, distribute and/or modify this document
 under the terms of the GNU Free Documentation License, Version 1.3
 or any later version published by the Free Software Foundation;
 with no Invariant Sections, no Front-Cover Texts, and no Back-Cover
 Texts. A copy of the license is included in the section entitled ‘‘GNU
 Free Documentation License’’.
   2019年 03月 14日 星期四 19:24:32 CST. 
*/
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 13日 星期三 08:54:03 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 08日 星期五 08:10:16 CST. */
/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 07日 星期四 20:27:53 CST. */
/*
 *	16.8.1 Creating a Socket
 *	The primitive for creating a socket is the socket function, declared in ‘sys/socket.h’.
 *	int socket (int namespace, int style, int protocol) [Function]
 *	This function creates a socket and specifes communication style style, which should be
 *	one of the socket styles listed in Section 16.2 [Communication Styles], page 390. The
 *	namespace argument specifes the namespace; it must be PF_LOCAL (see Section 16.5
 *	[The Local Namespace], page 395) or PF_INET (see Section 16.6 [The Internet Names
 *	pace], page 397). protocol designates the specifc protocol (see Section 16.1 [Socket
 *	Concepts], page 389); zero is usually right for protocol.
 *	The return value from socket is the fle descriptor for the new socket, or -1 in case
 *	of error. The following errno error conditions are defned for this function:
 *	EPROTONOSUPPORT
 *	The protocol or style is not supported by the namespace specifed.
 *	EMFILE The process already has too many fle descriptors open.
 *	ENFILE The system already has too many fle descriptors open.
 *	EACCES The process does not have the privilege to create a socket of the specifed
 *	style or protocol.
 *	ENOBUFS The system ran out of internal buﬀer space.
 *	The fle descriptor returned by the socket function supports both read and write
 *	operations. However, like pipes, sockets do not support fle positioning operations.
 *
 *	16.8.2 Closing a Socket
 *	When you have fnished using a socket, you can simply close its fle descriptor with close;
 *	see Section 13.1 [Opening and Closing Files], page 296. If there is still data waiting to be
 *	transmitted over the connection, normally close tries to complete this transmission. You
 *	can control this behavior using the SO_LINGER socket option to specify a timeout period;
 *	see Section 16.12 [Socket Options], page 429.
 *	You can also shut down only reception or transmission on a connection by calling
 *	shutdown, which is declared in ‘sys/socket.h’.
 *	int shutdown (int socket, int how) [Function]
 *	The shutdown function shuts down the connection of socket socket. The argument
 *	how specifes what action to perform:
 *	0 Stop receiving data for this socket. If further data arrives, reject it.
 *	1 Stop trying to transmit data from this socket. Discard any data waiting
 *	to be sent. Stop looking for acknowledgement of data already sent; don’t
 *	retransmit it if it is lost.
 *	2 Stop both reception and transmission.
 *	The return value is 0 on success and -1 on failure. The following errno error condi
 *	tions are defned for this function:
 *	EBADF socket is not a valid fle descriptor.
 *	ENOTSOCK socket is not a socket.
 *	ENOTCONN socket is not connected.
 */

#include <sys/socket.h>

int socket (int namespace, int style, int protocol);

/* The following constants should be used for the second parameter of
   `shutdown'.  */
enum
{
  SHUT_RD = 0,		/* No more receptions.  */
#define SHUT_RD		SHUT_RD
  SHUT_WR,		/* No more transmissions.  */
#define SHUT_WR		SHUT_WR
  SHUT_RDWR		/* No more receptions or transmissions.  */
#define SHUT_RDWR	SHUT_RDWR
};


int shutdown (int socket, int how);

//SHUT_RD:关闭读
//SHUT_WR:关闭写的这一半
//SHUT_RDWR:读写都关闭


/**
 *	终止网络连接的方法是close函数。不过close有两个限制：
 *		1.close把描述符的引用计数-1，仅在该计数变为0时才关闭套接字；
 *		2.close终止读和写两个方向的数据传送。
 */
int shutdown(int sockfd, int howto);

/**
 *	shutdown依赖于howto参数的值：
 *		
 *		SHUT_RD：关闭套接字读的这一半。
 *				套接字中不再有数据可接收，而且套接字接收缓冲区中的现有数据都被丢弃。
 *				进程不能再对这样的套接字调用任何读函数。对于TCP套接字这样调用
 *				shutdown函数后，由该套接字接收的来自对端的任何数据都被确认，然后
 *				悄然丢弃。
 *
 *		SHUT_WR：关闭连接的写的这一半。
 *				对于TCP套接字，这称为半关闭。
 *
 *		SHUT_RDWR：连接的读半部和写半部都关闭。
 *
 *
 *
 *
 *	SHUT_RD
 *		The same semantics as for TCP discussed in Section 6.6(See 8.4.6); no SCTP 
 *		protocol action is taken.
 *	SHUT_WR
 *		Disables further send operations and initiates the SCTP shutdown procedures, 
 *		which will terminate the association. Note that this option does not provide 
 *		a half-closed state, but does allow the local endpoint to read any queued data 
 *		that the peer may have sent prior to receiving the SCTP SHUTDOWN message.
 *	SHUT_RDWR
 *		Disables all read and write operations, and initiates the SCTP shutdown procedure. 
 *		Any queued data that was in transit to the local endpoint will be acknowledged and 
 *		then silently discarded.
 */

