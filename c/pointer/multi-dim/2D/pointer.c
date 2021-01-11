#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

int main()
{
	int arr[2][3] = {{1,2,3}, {4,5,6}};

	printf("%2d\n", *(*(arr+0)+1));

	return 0;
}
