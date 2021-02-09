/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 06月 14日 星期五 09:05:24 CST. */
/* 
 Copyright (C) Rong Tao @Beijing

 Permission is granted to copy, distribute and/or modify this document
 under the terms of the GNU Free Documentation License, Version 1.3
 or any later version published by the Free Software Foundation;
 with no Invariant Sections, no Front-Cover Texts, and no Back-Cover
 Texts. A copy of the license is included in the section entitled ‘‘GNU
 Free Documentation License’’.
   2019年 03月 14日 星期四 19:24:30 CST. 
*/
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 13日 星期三 08:54:01 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 08日 星期五 08:10:14 CST. */
/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 07日 星期四 20:27:52 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 07日 星期四 20:26:58 CST. */
/*
 *	ptrdiff_t [Data Type]
 *	This is the signed integer type of the result of subtracting two pointers. For example,
 *	with the declaration char *p1, *p2;, the expression p2 - p1 is of type ptrdiff_t.
 *	This will probably be one of the standard signed integer types (short int, int or
 *	long int), but might be a nonstandard type that exists only for this purpose.
 */
///usr/include/asm-generic/posix_types.h:69:
typedef int       __kernel_ptrdiff_t;

///usr/include/asm-generic/posix_types.h:73:
typedef __kernel_long_t   __kernel_ptrdiff_t;

///usr/include/nspr4/obsolete/protypes.h:139:
typedef PRUptrdiff uptrdiff_t;

///usr/include/tcl-private/generic/tclInt.h:54:
typedef int ptrdiff_t;

