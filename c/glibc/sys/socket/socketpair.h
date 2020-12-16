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
/**
 *	16.8.3 Socket Pairs
 *	A socket pair consists of a pair of connected (but unnamed) sockets. It is very similar to
 *	a pipe and is used in much the same way. Socket pairs are created with the socketpair
 *	function, declared in ‘sys/socket.h’. A socket pair is much like a pipe; the main diﬀerence
 *	is that the socket pair is bidirectional, whereas the pipe has one input-only end and one
 *	output-only end (see Chapter 15 [Pipes and FIFOs], page 384).
 *	int socketpair (int namespace, int style, int protocol, int [Function]
 *	filedes[2])
 *	This function creates a socket pair, returning the fle descriptors in filedes[0] and
 *	filedes[1]. The socket pair is a full-duplex communications channel, so that both
 *	reading and writing may be performed at either end.
 *	The namespace, style and protocol arguments are interpreted as for the socket func
 *	tion. style should be one of the communication styles listed in Section 16.2 [Commu
 *	nication Styles], page 390. The namespace argument specifes the namespace, which
 *	must be AF_LOCAL (see Section 16.5 [The Local Namespace], page 395); protocol
 *	specifes the communications protocol, but zero is the only meaningful value.
 *
 *	If style specifes a connectionless communication style, then the two sockets you get
 *	are not connected, strictly speaking, but each of them knows the other as the default
 *	destination address, so they can send packets to each other.
 *	The socketpair function returns 0 on success and -1 on failure. The following errno
 *	error conditions are defned for this function:
 *	EMFILE The process has too many fle descriptors open.
 *	EAFNOSUPPORT
 *	The specifed namespace is not supported.
 *	EPROTONOSUPPORT
 *	The specifed protocol is not supported.
 *	EOPNOTSUPP
 *	The specifed protocol does not support the creation of socket pairs.
 */
 
/**
 *	socketpair函数创建连个随后关联起来的套接字。
 *	本函数仅适用于Unix域套接字。
 */

#include <sys/socket.h>

int socketpair(int family, int type, int protocol, int sockfd[2]);
/**
 *	返回值：
 *		成功：0
 *		出错：-1
 *
 *	family必须为AF_LOCAL
 *	protocol参数必须为0；
 *	type参数既可以是SOCK_STREAM，也可以是SOCK_DGRAM。
 *	新创建的两个套接字以sockfd[0]和sockfd[1]返回。
 *
 *	本函数类似于Unix的pipe函数，会返回两个彼此连接的描述符，事实上，
 *	源自Berkeley的实现通过执行与socketpair一样的内部操作给出pipe接口。
 */
