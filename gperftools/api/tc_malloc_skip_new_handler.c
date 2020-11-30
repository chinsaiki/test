#include <gperftools/tcmalloc.h>
#include <stdio.h>

void* tc_malloc_skip_new_handler(size_t size);

int main()
{
    char *str = tc_malloc_skip_new_handler(1024);
    printf("%s\n", str);

    tc_free(str);
}

