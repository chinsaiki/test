#include <gperftools/tcmalloc.h>
#include <stdio.h>
#include <string.h>

//void* tc_malloc(size_t size);
//void tc_free(void* ptr);
//void* tc_valloc(size_t __size);
//void* tc_pvalloc(size_t __size);
//void tc_malloc_stats(void);
int tc_mallopt(int cmd, int value);


//int main()
//{
//    char *str = tc_pvalloc(1024);
//    printf("%s\n", str);
//
//    tc_malloc_stats();
//
//    tc_free(str);
//}


//需要使用 -lmcheck 编译
//
//    gcc mallopt.c -lmcheck


int main(int argc, char * argv[])
{
    int ret = mallopt(M_CHECK_ACTION, 3);
    printf("mallopt ret = %d\n", ret);
    
    const char * src = "mcheck test";
    char * ptr = (char *)tc_malloc(sizeof(char) * 32);
    
    printf("tc_malloc_size = %d\n", tc_malloc_size(ptr));

    
    memcpy(ptr, src, strlen(src));
    ptr[32] = '\0';  // 这里在堆内存块尾部越界 ！！！！！！！这里好像不行！！！！！！
    printf("*ptr=%s\n", ptr);
    
    tc_free(ptr);
    ptr = NULL;
    printf("process end\n");
 
    return 0;
}


