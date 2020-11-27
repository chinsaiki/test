#include <gperftools/tcmalloc.h>
#include <stdio.h>


int main()
{
    char *str = tc_malloc(1024);
    printf("%s\n", str);
}
