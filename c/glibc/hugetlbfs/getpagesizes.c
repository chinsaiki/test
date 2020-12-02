#include <stdio.h>
#include <hugetlbfs.h>

//4096. 4 kB, 0 MB
//2097152. 2048 kB, 2 MB
//1073741824. 1048576 kB, 1024 MB

int getpagesizes(long pagesizes[], int n_elem);

int main()
{
	int i;
	long pagesizes[64];
	int n_elem=64;
	int ret = getpagesizes(pagesizes, n_elem);

	printf("getpagesizes ret=%d.\n");

	for(i=0;i<ret;i++){
		printf("%ld. %ld kB, %ld MB\n", pagesizes[i], pagesizes[i]/1024, pagesizes[i]/1024/1024);
	}
}

