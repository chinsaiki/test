#include <stdio.h>
#include <stdio.h>
#include <mcheck.h>
#include <malloc.h>
#include <string.h>

void print_mstatus(enum mcheck_status mstatus){
    switch(mstatus){
        case MCHECK_DISABLED:
            printf("consistency checking is not turned on\n");
            break;
        case MCHECK_OK:
            printf("block is fine\n");
            break;
        case MCHECK_FREE:
            printf("block freed twice\n");
            break;
        case MCHECK_HEAD:
            printf("memory before the block was clobbered\n");
            break;
        case MCHECK_TAIL:
            printf("memory after the block was clobbered\n");
            break;
        default:
            printf("Memory is OK.\n");
            break;
    }
}


/*888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888*/

//加上 -lmcheck 进行编译
// gcc -lmcheck -g -o demo main.c
int test1_malloc()
{
    const char * src = "mcheck test";
    char * ptr = (char *)malloc(sizeof(char) * 32);
    memcpy(ptr - 1, src, strlen(src));  // 这里在堆内存块头部越界
    ptr[strlen(src) - 1] = '\0';
    printf("*ptr=%s\n", ptr);
 
    free(ptr);
    ptr = NULL;
    printf("process end\n");
 
    return 0;
}

/*888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888*/
//不可使用 -lmcheck 进行编译
// gcc -g -o demo main.c

void mcheck_func_cb(enum mcheck_status mc_status)
{
    printf("\n[%s:%d] mcheck_status=%d\n", __FILE__, __LINE__, mc_status);
    print_mstatus(mc_status);
}
 
int test2_mcheck()
{
    int ret = mcheck(mcheck_func_cb);
    if(0 != ret) {
        printf("[%s:%d] mcheck() failed\n",  __FILE__, __LINE__);
        return -1; 
    }   
 
    const char * src = "mcheck test";
    char * ptr = (char *)malloc(sizeof(char) * 32);
    memcpy(ptr - 1, src, strlen(src));  // 这里在堆内存块头部越界
    ptr[strlen(src) - 1] = '\0';
    printf("*ptr=%s\n", ptr);
     
    free(ptr);
    ptr = NULL;
    printf("process end\n");
 
    return 0;
}

int main(int argc, char * argv[])
{
//    test1_malloc();
    test2_mcheck();
}

