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
 *	16.9.2 Listening for Connections
 *	Now let us consider what the server process must do to accept connections on a socket. First
 *	it must use the listen function to enable connection requests on the socket, and then accept
 *	each incoming connection with a call to accept (see Section 16.9.3 [Accepting Connections],
 *	page 415). Once connection requests are enabled on a server socket, the select function
 *	reports when the socket has a connection ready to be accepted (see Section 13.8 [Waiting
 *	for Input or Output], page 313).
 *	The listen function is not allowed for sockets using connectionless communication
 *	styles.
 *	You can write a network server that does not even start running until a connection to it
 *	is requested. See Section 16.11.1 [inetd Servers], page 428.
 *	In the Internet namespace, there are no special protection mechanisms for controlling
 *	access to a port; any process on any machine can make a connection to your server. If
 *	you want to restrict access to your server, make it examine the addresses associated with
 *	connection requests or implement some other handshaking or identifcation protocol.
 *	In the local namespace, the ordinary fle protection bits control who has access to connect
 *	to the socket.
 *	int listen (int socket, int n) [Function]
 *	The listen function enables the socket socket to accept connections, thus making it
 *	a server socket.
 *	The argument n specifes the length of the queue for pending connections. When the
 *	queue flls, new clients attempting to connect fail with ECONNREFUSED until the server
 *	calls accept to accept a connection from the queue.
 *	The listen function returns 0 on success and -1 on failure. The following errno
 *	error conditions are defned for this function:
 *	EBADF The argument socket is not a valid fle descriptor.
 *	ENOTSOCK The argument socket is not a socket.
 *	EOPNOTSUPP
 *	The socket socket does not support this operation.
 *
 *
 *
 *  使其能够自动接收到来的连接并且为连接队列指定一个长度限制.  之后就可以使用 
 */
#include <sys/socket.h>

int listen (int socket, int n);

int main()
{
	return 0;
}
