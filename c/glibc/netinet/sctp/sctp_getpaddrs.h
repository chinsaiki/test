/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 06月 14日 星期五 09:05:19 CST. */
/* 
 Copyright (C) Rong Tao @Beijing

 Permission is granted to copy, distribute and/or modify this document
 under the terms of the GNU Free Documentation License, Version 1.3
 or any later version published by the Free Software Foundation;
 with no Invariant Sections, no Front-Cover Texts, and no Back-Cover
 Texts. A copy of the license is included in the section entitled ‘‘GNU
 Free Documentation License’’.
   2019年 03月 14日 星期四 19:24:28 CST. 
*/
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 13日 星期三 08:53:58 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 11日 星期一 08:57:54 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 08日 星期五 08:10:12 CST. */
/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 07日 星期四 20:27:50 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 07日 星期四 20:26:56 CST. */
//The getpeername function was not designed with the concept of a multihoming-aware transport protocol; when using SCTP, it only returns the primary address. When all the addresses are required, the sctp_getpaddrs function provides a mechanism for an application to retrieve all the addresses of a peer.

#include <netinet/sctp.h>


/**
 *	getpeername函数用于sctp中时，仅仅返回主目的地址。
 *	如果需要知道对端的所有地址，就用这个函数就行了。
 *
 *	返回值：
 *		成功：存放在addrs中的对端地址数；
 *		错误：-1
 */
int sctp_getpaddrs(int sockfd, sctp_assoc_t id, struct sockaddr **addrs);
//Returns: the number of peer addresses stored in addrs, –1 on error




