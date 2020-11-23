#include <stdio.h>
#include <stdlib.h>
#include <mcheck.h>
#include <errno.h>
#include <string.h>

struct test_struct{
    int id;
};

void abortfun(enum mcheck_status mstatus)
{
	fprintf(stderr, "abortfun called with %d\n", mstatus);
    switch(mstatus){
        case MCHECK_DISABLED:
            printf("consistency checking is not turned on\n");
            break;
        case MCHECK_OK:
            printf("block is fine\n");
            break;
        case MCHECK_FREE:
            printf("block freed twice\n");
            break;
        case MCHECK_HEAD:
            printf("memory before the block was clobbered\n");
            break;
        case MCHECK_TAIL:
            printf("memory after the block was clobbered\n");
            break;
    }
    exit(1); //这里不退出，将打印出backtrace
    //    *** Error in `./a.out': double free or corruption (fasttop): 0x0000000001548030 ***
    //    ======= Backtrace: =========
    //    /usr/lib64/libc.so.6(+0x81299)[0x7f77bd17e299]
    //    /usr/lib64/libc.so.6(+0x892f8)[0x7f77bd1862f8]
    //    ./a.out[0x40082e]
    //    /usr/lib64/libc.so.6(__libc_start_main+0xf5)[0x7f77bd11f555]
    //    ./a.out[0x400669]
}

int main(int argc, char *argv[])
{
	struct test_struct *p = NULL;

    //比 mcheck 更低效，但更详细的
	if(mcheck_pedantic(abortfun) != 0)
	{
		fprintf(stderr, "mcheck:%s\n", strerror(errno));
		return -1;
	}
	p = malloc(sizeof(struct test_struct));
	free(p);
	printf("1st free finished.\n");
	free(p);
	printf("2nd free.\n");

	return 0;
}


