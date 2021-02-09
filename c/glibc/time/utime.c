/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 06月 14日 星期五 09:05:33 CST. */
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
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 13日 星期三 08:54:06 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 08日 星期五 08:10:18 CST. */
/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 07日 星期四 20:27:55 CST. */
/*	int utime (const char *filename, const struct utimbuf *times) [Function]
 *	This function is used to modify the fle times associated with the fle named flename.
 *	If times is a null pointer, then the access and modifcation times of the fle are set to
 *	the current time. Otherwise, they are set to the values from the actime and modtime
 *	members (respectively) of the utimbuf structure pointed to by times.
 *	The attribute modifcation time for the fle is set to the current time in either case
 *	(since changing the time stamps is itself a modifcation of the fle attributes).
 *	The utime function returns 0 if successful and -1 on failure. In addition to the usual
 *	fle name errors (see Section 11.2.3 [File Name Errors], page 224), the following errno
 *	error conditions are defned for this function:
 *	EACCES There is a permission problem in the case where a null pointer was passed
 *	as the times argument. In order to update the time stamp on the fle,
 *	you must either be the owner of the fle, have write permission for the
 *	fle, or be a privileged user.
 *	ENOENT The fle doesn’t exist.
 *	EPERM If the times argument is not a null pointer, you must either be the owner
 *	of the fle or be a privileged user.
 *	EROFS The fle lives on a read-only fle system.
 */
#include <time.h>
#include <stdio.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
	struct utimbuf utim;

	utim.actime = 12;
	utim.modtime = 13;

	utime(argv[1], &utim);

	return 0;
}
