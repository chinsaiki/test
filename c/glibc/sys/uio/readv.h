/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 06月 14日 星期五 09:05:31 CST. */
/* 
 Copyright (C) Rong Tao @Beijing

 Permission is granted to copy, distribute and/or modify this document
 under the terms of the GNU Free Documentation License, Version 1.3
 or any later version published by the Free Software Foundation;
 with no Invariant Sections, no Front-Cover Texts, and no Back-Cover
 Texts. A copy of the license is included in the section entitled ‘‘GNU
 Free Documentation License’’.
   2019年 03月 14日 星期四 19:24:34 CST. 
*/
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 13日 星期三 08:54:05 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 08日 星期五 08:10:18 CST. */
/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 07日 星期四 20:27:54 CST. */
//These two functions are similar to read and write, but readv and writev let us read into or write from one or more buffers with a single function call. These operations are called scatter read (since the input data is scattered into multiple application buffers) and gather write (since multiple buffers are gathered for a single output operation).
//

#include <sys/uio.h>

struct iovec {
	void *iov_base; /* starting address of buffer */
	size_t iov_len; /* size of buffer */
};


ssize_t readv(int filedes, const struct iovec *iov, int iovcnt);
//ssize_t writev(int filedes, const struct iovec *iov, int iovcnt);
//Both return: number of bytes read or written, –1 on error



//The datatypes shown for the members of the iovec structure are those specified by POSIX. You may encounter implementations that define iov_base to be a char *, and iov_len to be an int.
//There is some limit to the number of elements in the array of iovec structures that an implementation allows. Linux, for example, allows up to 1,024, while HP-UX has a limit of 2,100. POSIX requires that the constant IOV_MAX be defined by including the <sys/uio.h> header and that its value be at least 16.
//The readv and writev functions can be used with any descriptor, not just sockets. Also, writev is an atomic operation. For a record-based protocol such as UDP, one call to writev generates a single UDP datagram.
//We mentioned one use of writev with the TCP_NODELAY socket option in Section 7.9(See 8.5.9). We said that a write of 4 bytes followed by a write of 396 bytes could invoke the Nagle algorithm and a preferred solution is to call writev for the two buffers.


