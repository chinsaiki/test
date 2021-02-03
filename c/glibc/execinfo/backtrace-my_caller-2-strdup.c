#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <execinfo.h>


#define BACK_TRACE_BUFF_SIZE 100

char* my_caller(int size,unsigned int depth) 
{
    char *name = NULL;
    int j=0, nptrs=0;
    void *buffer[BACK_TRACE_BUFF_SIZE];
    char **strings;
    char *start = NULL,*end= NULL,n = 0;

    nptrs = backtrace(buffer, BACK_TRACE_BUFF_SIZE);
    /* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
       would produce similar output to the following: */

    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
        perror("backtrace_symbols");
        return NULL;
    }
//    for(j=0;j<nptrs;j++) {
//        printf("strings[%d] = %s\n", j, strings[j]);
//    }
//    free(strings);
    j = (nptrs > 2 ? 2 : 0);
    j+=depth;

    if(j >= nptrs) {
        free(strings);
        return NULL;
    }
    printf("strings[j] = %s\n", strings[j]);
    start = strstr(strings[j],"(")+1;
    end = strstr(strings[j],"+");

    n = end - start + 1;
    n = n > size ? size : n;
    name = strndup(start, n);
    free(strings);
    return name;
    return "##########################";
}

int demo1()
{
    char *caller = my_caller(256, 1);
    printf("%s caller is %s\n", __func__, caller);
    
    if(caller) free(caller);
}


int demo2()
{
    char *caller = my_caller(256, 1);
    printf("%s caller is %s\n", __func__, caller);
    
    if(caller) free(caller);
    
    demo1();
}

int main()
{
    demo1();
    demo2();

}


