#include <stdio.h>
#include <hugetlbfs.h>

int gethugepagesizes(long pagesizes[], int n_elem);

int main()
{
	long ret = gethugepagesize();

	printf("gethugepagesizes ret=%d.\n", ret);
}

