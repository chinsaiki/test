#include <stdio.h>
#include <hugetlbfs.h>


const char *hugetlbfs_find_path(void);

int main()
{
	const char *path = hugetlbfs_find_path();

	printf("hugetlbfs_find_path path=%s.\n", path);
}



