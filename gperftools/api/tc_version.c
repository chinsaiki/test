#include <gperftools/tcmalloc.h>
#include <stdio.h>

const char* tc_version(int* major, int* minor, const char** patch);

int main()
{
    int major;
    int minor;
    const char* patch;

    const char *str = tc_version(&major, &minor, &patch);
    printf("tcmalloc-%d.%d%s patch:%s \n", major, minor, patch, str);

}

