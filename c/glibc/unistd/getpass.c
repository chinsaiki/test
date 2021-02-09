/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 09月 19日 星期四 08:43:11 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 06月 14日 星期五 09:05:34 CST. */
/* 
 Copyright (C) Rong Tao @Beijing

 Permission is granted to copy, distribute and/or modify this document
 under the terms of the GNU Free Documentation License, Version 1.3
 or any later version published by the Free Software Foundation;
 with no Invariant Sections, no Front-Cover Texts, and no Back-Cover
 Texts. A copy of the license is included in the section entitled ‘‘GNU
 Free Documentation License’’.
   2019年 03月 14日 星期四 19:24:35 CST. 
*/
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 13日 星期三 08:54:07 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 08日 星期五 08:10:19 CST. */
/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 07日 星期四 20:27:56 CST. */
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

ssize_t my_getpass(char **lineptr, size_t *n, FILE *stream)
{
	struct termios old, new;
	int nread;
	/* Turn echoing oﬀ and fail if we can’t. */
	if (tcgetattr (fileno (stream), &old) != 0)
		return -1;
	new = old;
	new.c_lflag &= ~ECHO;
	if (tcsetattr (fileno (stream), TCSAFLUSH, &new) != 0)
		return -1;
	/* Read the password. */
	nread = getline (lineptr, n, stream);
	/* Restore terminal. */
	(void) tcsetattr (fileno (stream), TCSAFLUSH, &old);
	return nread;
}

int main()
{
	/*	char * getpass (const char *prompt) [Function]
	 *	getpass outputs prompt, then reads a string in from the terminal without echoing
	 *	it. It tries to connect to the real terminal, ‘/dev/tty’, if possible, to encourage users
	 *	not to put plaintext passwords in fles; otherwise, it uses stdin and stderr. getpass
	 *	also disables the INTR, QUIT, and SUSP characters on the terminal using the ISIG
	 *	terminal attribute (see Section 17.4.7 [Local Modes], page 442). The terminal is
	 *	ﬂushed before and after getpass, so that characters of a mistyped password are not
	 *	accidentally visible.
	 *	In other C libraries, getpass may only return the frst PASS_MAX bytes of a password.
	 *	The GNU C library has no limit, so PASS_MAX is undefned.
	 *	The prototype for this function is in ‘unistd.h’. PASS_MAX would be defned in
	 *	‘limits.h’.
	 */
	char prompt[256];

	memset(prompt, 0, sizeof(prompt));

	sprintf(prompt, "password:");
	char *ret = malloc(256);;


	unsigned int try_times = 0;
	while(strncmp(ret = getpass(prompt), "rong ", strlen(ret)>5?strlen(ret):5) != 0)
	{
		try_times++;
		printf(" try again:");
		if(try_times > 2)
		{
			printf("Good bye.\n");
			exit(1);
		}
	}

	printf("%s\n", ret);

	return 0;
}
