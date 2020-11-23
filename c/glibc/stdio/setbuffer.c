/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 06月 14日 星期五 09:05:24 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 04月 02日 星期二 08:54:30 CST. */
#include <stdio.h>

char outbuf[BUFSIZ/*8192*/];

int main()
{
	char string[256] = {"Hello world!"};
	
	//setbuffer(stdout, string, sizeof(string));
	setbuffer(stdout, outbuf, sizeof(string));
	fflush(stdout);

	fprintf(stdout, "rongtao: %s\n", "Rongtao shinidaye");

	printf("%s: %d\n", string, BUFSIZ);

	printf("---------\n");
	printf("outbuf: %s\n", outbuf);
	/**
	 *	$ gcc setbuf.c 
	 *	[rongtao@localhost stdio]$ ./a.out 
	 *	rongtao: Rongtao shinidaye
	 *
	 *	onggao shinn
	 */
	return 0;
}

