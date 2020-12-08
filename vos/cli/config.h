#ifndef __config_h
#define __config_h 1

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>


#define VOS_OK     (0)
#define VOS_ERROR (-1)

#define VOS_SYSNAME_LEN 64

#define _CL_VECTOR_MIN_SIZE_ 1
#define _CL_VTY_TERMLENGTH_DEFAULT_ 25

#define _CL_VTY_TELNET_ 0
#define _CL_VTY_CONSOLE_ 1
#define _CL_VTY_SSH_ 2
#define _CL_VTY_TELNET_V6_ 3

#define _CL_VTY_TIMEOUT_LOGIN_        60*3
#define _CL_VTY_TIMEOUT_DEFAULT_     0/*1200*/
#define _CL_VTY_TERMLENGTH_DEFAULT_ 25


#define VOS_Sprintf sprintf
#define VOS_MemSet memset
#define VOS_Vsnprintf vsnprintf
#define VOS_MemZero(p, size) memset(p, 0, size)
#define VOS_StrCpy strcpy
#define VOS_Malloc(size, mod) malloc(size)
#define VOS_Free free

#define VOS_ASSERT assert

typedef int INT;
typedef char CHAR;
typedef long LONG;
typedef void VOID;
typedef void *VOS_HANDLE;
typedef unsigned long ULONG;
typedef bool BOOL;


extern INT VOS_Console_FD;



void sys_console_init ( void );


#endif //__config_h
