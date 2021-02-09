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
 *	size_t [Data Type]
 *	This is an unsigned integer type used to represent the sizes of objects. The result of
 *	the sizeof operator is of this type, and functions such as malloc (see Section 3.2.2
 *	[Unconstrained Allocation], page 33) and memcpy (see Section 5.4 [Copying and Con
 *	catenation], page 77) accept arguments of this type to specify object sizes.
 *	Usage Note: size_t is the preferred way to declare any arguments or variables that
 *	hold the size of an object.
 */

///usr/include/glob.h:30:
typedef __SIZE_TYPE__ size_t;

///usr/include/jsoncpp/json/value.h:658:  
typedef unsigned int size_t;

///usr/include/jsoncpp/json/value.h:725:  
//typedef unsigned int size_t;

///usr/include/jsoncpp/json/value.h:774:  
typedef unsigned int size_t;

