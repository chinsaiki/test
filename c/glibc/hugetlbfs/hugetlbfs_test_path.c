#include <stdio.h>
#include <hugetlbfs.h>

//On  success,  1  is returned for a hugetlbfs filesystem, 
//    or 0 for a non-hugetlbfs filesystem.  
//On failure, -1 is returned and errno is set appropriately.
int hugetlbfs_test_path(const char *mount);

void printf_hugetlbfs_test_path(const char *mount)
{   
	int ret = hugetlbfs_test_path(mount);

	printf("hugetlbfs_test_path %s - %d.\n", mount, ret);
}

int main()
{
	int i;
	long pagesizes[64];
	int n_elem=64;
    
    printf_hugetlbfs_test_path("/mnt/huge");
    printf_hugetlbfs_test_path("/dev/hugepages");

}


