/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 06月 14日 星期五 09:05:32 CST. */
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

/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 07日 星期四 20:27:55 CST. */
/*
 *	int setlogmask (int mask) [Function]
 *	setlogmask sets a mask (the “logmask”) that determines which future syslog calls
 *	shall be ignored. If a program has not called setlogmask, syslog doesn’t ignore any
 *	calls. You can use setlogmask to specify that messages of particular priorities shall
 *	be ignored in the future.
 *	A setlogmask call overrides any previous setlogmask call.
 *	Note that the logmask exists entirely independently of opening and closing of Syslog
 *	connections.
 *	Setting the logmask has a similar eﬀect to, but is not the same as, confguring Syslog.
 *	The Syslog confguration may cause Syslog to discard certain messages it receives,
 *	but the logmask causes certain messages never to get submitted to Syslog in the frst
 *	place.
 *	mask is a bit string with one bit corresponding to each of the possible message pri
 *	orities. If the bit is on, syslog handles messages of that priority normally. If it
 *	is oﬀ, syslog discards messages of that priority. Use the message priority macros
 *	described in Section 18.2.2 [syslog, vsyslog], page 462 and the LOG_MASK to construct
 *	an appropriate mask value, as in this example:
 *	LOG_MASK(LOG_EMERG) | LOG_MASK(LOG_ERROR)
 *	or
 *	~(LOG_MASK(LOG_INFO))
 *	There is also a LOG_UPTO macro, which generates a mask with the bits on for a certain
 *	priority and all priorities above it:
 *	LOG_UPTO(LOG_ERROR)
 *	The unfortunate naming of the macro is due to the fact that internally, higher numbers
 *	are used for lower message priorities.
 */
#include <syslog.h>

int setlogmask (int mask);
