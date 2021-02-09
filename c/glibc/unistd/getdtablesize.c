#include <stdio.h>
#include <unistd.h>

int main()
{
    int ret = getdtablesize();
    printf("getdtablesize = %d\n", ret);
    
	return 0;
}

