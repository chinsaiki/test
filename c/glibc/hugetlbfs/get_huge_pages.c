#include <stdio.h>
#include <hugetlbfs.h>

/* Allocation functions for regions backed by hugepages */
void *get_hugepage_region(size_t len, ghr_t flags);
void free_hugepage_region(void *ptr);

int main()
{
    char *hugepage = NULL;
    
    hugepage = get_hugepage_region(gethugepagesize(), GHP_DEFAULT);

    printf("%p\n", hugepage);

    free_hugepage_region(hugepage);
}

