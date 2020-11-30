#include <gperftools/tcmalloc.h>
#include <stdio.h>

//void* tc_malloc(size_t size);
//void tc_free(void* ptr);
void* tc_valloc(size_t __size);


int main()
{
    char *str = tc_valloc(1024);
    printf("%s\n", str);

    tc_free(str);
}

