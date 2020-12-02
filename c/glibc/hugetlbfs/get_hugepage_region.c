#include <stdio.h>
#include <hugetlbfs.h>

void *get_huge_pages(size_t len, ghp_t flags);
void free_huge_pages(void *ptr);

int main()
{
    char *hugepage = NULL;
    
    hugepage = get_huge_pages(gethugepagesize(), GHP_DEFAULT);

    printf("%p\n", hugepage);

    free_huge_pages(hugepage);
}


