/************************************************************************************************************\
 *
 *  hpmalloc - 高性能内存分配器
 *
 *  作者: 荣涛
 *  日期:   2020年12月
 *
 ************************************************************************************************************
 *
 *  高性能内存分配接口 
 *      -------------------------------------------------------------------
 *      void *hpmalloc(size_t size, module_id_t module_id);
 *      void *hpcalloc(size_t number, size_t size, module_id_t module_id);
 *      void *hprealloc(void *ptr, size_t size, module_id_t module_id);
 *      void hpfree(void *ptr);
 *      char *hpstrdup(const char *s, module_id_t module_id);
 *
 *  高性能检测DEBUG类接口
 *      -------------------------------------------------------------------
 *      int hpmemcheck(void *addr);
 *
 *  高性能统计类接口
 *      -------------------------------------------------------------------
 *      size_t hpmalloc_used_memory(void);
 *      size_t hpmalloc_module_used_memory(module_id_t module_id);
 *      size_t hpmsize(void *const ptr);
 *
 *  
 *  统计类接口
 *      -------------------------------------------------------------------
 *      size_t hpmalloc_get_memory_size(void);
 *      size_t hpmalloc_get_private_dirty(long pid);
 *      size_t hpmalloc_get_shared_dirty(long pid);size_t hpmalloc_get_rss(void);
 *      int hpmalloc_get_allocator_info(size_t *allocated, size_t *active, size_t *resident);
 *      size_t hpmalloc_get_rss(void);
 *  
\************************************************************************************************************/

#ifndef __HPMALLOC_H
#define __HPMALLOC_H 1

#include <sys/types.h>

/* 使用的内存分配器类型 */
typedef enum {
    HP_TCMALLOC,    /* tcmalloc */
    HP_JEMALLOC,    /* jemalloc */
    HP_PTMALLOC,    /* ptmalloc */
}HPMALLOC_TYPE;

/* 错误码 */
typedef enum {
    HPMEM_EOK,      /* fine */
    HPMEM_EPARAM,   /* 参数错误 */
    HPMEM_EHEAD,    /* 申请的内存块头部越界 */
    HPMEM_ETAIL,    /* 申请的内存块尾部越界 */
    HPMEM_ENOTSUPPORT,/* 不支持的操作 */
    HPMEM_EMAX,   
}hpmem_errno_t;

typedef unsigned long module_id_t; /* 模块ID */


/**
 *  hpmalloc_version 获取版本信息
 */
const char* hpmalloc_version(void);

/**
 *  hpmalloc_type 分配器类型
 */
HPMALLOC_TYPE hpmalloc_type(void);

/**
 *  hpmalloc_strerror 分配器错误字符串
 *
 *  errNo 错误码
 */
const char*const hpmalloc_strerror(hpmem_errno_t errNo);


/**
 *  hpmalloc 内存申请
 *  
 *  参数
 *  size        内存申请大小 Bytes
 *  module_id   在debug模式下，该字段用于 记录 模块ID 使用的内存大小
 *  
 *  返回值       内存地址
 */
void *hpmalloc(size_t size, module_id_t module_id);

/**
 *  hpcalloc 内存申请
 *  
 *  参数
 *  number      个数
 *  size        内存申请大小 Bytes
 *  module_id   在 debug 模式下，该字段用于 记录 模块ID 使用的内存大小
 *  
 *  返回值       内存空间首地址
 */
void *hpcalloc(size_t number, size_t size, module_id_t module_id);

/**
 *  hprealloc 重申请内存
 *  
 *  参数
 *  ptr         使用 hpmalloc 或 hpcalloc 申请的内存地址
 *  size        内存申请大小 Bytes
 *  module_id   在 debug 模式下，该字段用于 记录 模块ID 使用的内存大小
 *  
 *  返回值       内存空间首地址
 */
void *hprealloc(void *ptr, size_t size, module_id_t module_id);

/**
 *  hpfree 内存释放
 *  
 *  参数
 *  ptr         使用 hpmalloc 或 hpcalloc 申请的内存地址
 *  
 *  返回值 无
 */
void hpfree(void *ptr);

/**
 *  hpstrdup 复制字符串
 *  
 *  参数
 *  s   源字符串
 *  module_id   在 debug 模式下，该字段用于 记录 模块ID 使用的内存大小
 *  
 *  返回值       复制的字符串的首地址
 */
char *hpstrdup(const char *s, module_id_t module_id);

/**
 *  hpmemcheck 检测该块内存的使用情况，包括越界检测
 *  
 *  参数
 *  addr   申请的内存首地址
 *  
 *  返回值       错误码 hpmem_errno_t
 */
int hpmemcheck(void *addr);

/**
 *  hpmsize 申请的内存块大小
 *  
 *  ptr 通过内存分配函数申请的内存地址
 *  
 *  返回      内存大小
 */
size_t hpmsize(void *const ptr);


/**
 *  hpmalloc_used_memory 获取使用的内存大小
 *  
 *  返回      申请的内存总大小
 */
size_t hpmalloc_used_memory(void);

/**
 *  hpmalloc_module_used_memory 获取（模块）使用的内存大小
 *  
 *  module_id 模块ID
 *
 *  返回      申请的内存总大小
 */
size_t hpmalloc_module_used_memory(module_id_t module_id);

/**
 *  设置 Out Of Memory 回调函数
 */
void hpmalloc_set_oom_handler(void (*oom_handler)(size_t));

/**
 *  hpmalloc_get_rss 获取实际使用的物理内存大小（包含共享内存库）
 *
 *   实际使用的物理内存（包含共享库占用的内存）
 *
 * 注: RSS Resident Set Size，该接口不考虑性能，不应在代码中周期调用
 *
 */
size_t hpmalloc_get_rss(void);


/**
 *  hpmalloc_get_allocator_info 获取总内存分配和剩余大小，采用不同的分配器，值会有所差异
 *  
 *  allocated   申请的内存大小
 *  active      激活的内存大小
 *  resident    剩余内存大小
 */
int hpmalloc_get_allocator_info(size_t *allocated, size_t *active, size_t *resident);

/**
 *  hpmalloc_set_jemalloc_bg_thread 设置jemalloc后台的内存分配器管理线程数使能
 *  
 *  enable 为真 使能， 否则，无效。
 *
 *  返回 hpmem_errno_t
 */
int hpmalloc_set_jemalloc_bg_thread(int enable);

/**
 *  hpmalloc_jemalloc_purge 将长时间不用的 page 归还给 系统内核
 *
 *  返回 hpmem_errno_t
 */
int hpmalloc_jemalloc_purge();

/**
 *  hpmalloc_get_private_dirty 获取私有脏页大小(bytes)
 */
size_t hpmalloc_get_private_dirty(long pid);

/**
 *  hpmalloc_get_shared_dirty 获共享有脏页大小(bytes)
 */
size_t hpmalloc_get_shared_dirty(long pid);

/**
 *  hpmalloc_get_memory_size 返回物理内存RAM大小
 */
size_t hpmalloc_get_memory_size(void);

/**
 *  hplibc_free 当采用 backtrace_symbols 等接口时，需要使用 libc 的 free 接口
 */
void hplibc_free(void *ptr);



#endif /* __HPMALLOC_H */
