#include <stdio.h>
#include <malloc.h>
#include <string.h>

//需要使用 -lmcheck 编译
//
//    gcc mallopt.c -lmcheck


int main(int argc, char * argv[])
{
    int ret = mallopt(M_CHECK_ACTION, 3);
    printf("mallopt ret = %d\n", ret);
    
    const char * src = "mcheck test";
    char * ptr = (char *)malloc(sizeof(char) * 32);

    
    memcpy(ptr, src, strlen(src));
    ptr[32] = '\0';  // 这里在堆内存块尾部越界
    printf("*ptr=%s\n", ptr);
    
    free(ptr);
    ptr = NULL;
    printf("process end\n");
 
    return 0;
}

