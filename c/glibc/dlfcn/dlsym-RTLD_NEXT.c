#include <stdio.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <sys/un.h>
#include <netinet/in.h>

#ifndef __cplusplus
#define __USE_GNU
#endif
#include <dlfcn.h>
#include <unistd.h>

typedef int (*socket_pfn_t)(int domain, int type, int protocol);

static socket_pfn_t g_sys_socket_func = NULL;

#define HOOK_SYS_FUNC(name) \
    if( !g_sys_##name##_func ) { \
        g_sys_##name##_func = (name##_pfn_t)dlsym(RTLD_NEXT,#name);\
    }

void __attribute__((constructor(101))) __dlsym_sys_func_init()
{
    g_sys_socket_func = (socket_pfn_t)dlsym(RTLD_NEXT,"socket");
}

int socket(int domain, int type, int protocol)
{
	HOOK_SYS_FUNC( socket );

    printf("call socket.\n");
        
    int fd = g_sys_socket_func(domain, type, protocol);

	return fd;
}

//1.创建 test.c 文件，内容如下
//    #include <sys/socket.h>
//
//    int main()
//    {
//        int fd = socket(1,1,1);
//    }

//2.加载 socket冲定义的 .o 文件
//    gcc dlsym-RTLD_NEXT.c  -ldl -c

//3.编译
//    gcc test.c dlsym-RTLD_NEXT.o -ldl
//[rongtao@localhost dlfcn]$ gcc dlsym-RTLD_NEXT.c -c
//[rongtao@localhost dlfcn]$ gcc test.c dlsym-RTLD_NEXT.o -ldl
//[rongtao@localhost dlfcn]$ ./a.out 
//call socket.
//[rongtao@localhost dlfcn]$ gcc test.c 
//[rongtao@localhost dlfcn]$ ./a.out 
//[rongtao@localhost dlfcn]$

