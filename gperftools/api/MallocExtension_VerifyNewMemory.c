#include <gperftools/tcmalloc.h>
#include <stdio.h>

void* tc_malloc(size_t size);
void tc_free(void* ptr);
//int MallocExtension_VerifyAllMemory(void);
int MallocExtension_VerifyNewMemory(const void* p);


int main()
{
    char *str = tc_malloc(1024);
    printf("%s\n", str);

    int ret = MallocExtension_VerifyAllMemory();
    printf("%d\n", ret);

    ret = MallocExtension_VerifyNewMemory(str);
    printf("%d\n", ret);

    ret = MallocExtension_VerifyNewMemory(NULL);
    printf("%d\n", ret);

    tc_free(str);
}


