#include <jemalloc/jemalloc.h>

//标准接口
//---------------------------------------------------------------------------------------------------------

void *malloc(size_t size);

void *calloc(size_t number, size_t size);

int posix_memalign(void **ptr, size_t alignment, size_t size);

void *aligned_alloc(size_t alignment, size_t size);

void *realloc(void *ptr, size_t size);

void free(void *ptr);

//非标准接口
//---------------------------------------------------------------------------------------------------------
void *mallocx(size_t size, int flags);

//重新申请内存，可大可小
void *rallocx(void *ptr, size_t size, int flags);

//扩展内存空间
size_t xallocx(void *ptr, size_t size, size_t extra, int flags);

//返回申请的内存大小
size_t sallocx(void *ptr, int flags);

//引用，以便后面申请
void dallocx(void *ptr, int flags);

void sdallocx(void *ptr, size_t size, int flags);

//不申请内存，返回 调用 mallocx 的长度值
size_t nallocx(size_t size, int flags);

//控制malloc参数，用mib的形式，其中可以添加数字节点，例如 "arenas.bin.0.size"
int mallctl(const char *name, void *oldp, size_t *oldlenp, void *newp, size_t newlen);
int mallctlnametomib(const char *name, size_t *mibp, size_t *miblenp);
int mallctlbymib(const size_t *mib, size_t miblen, void *oldp, size_t *oldlenp, void *newp, size_t newlen);

//打印内存申请统计信息
//“J” JSON format[2]. 
//“g” General information that never changes during execution
//“m” merged arena
//“d” destroyed merged
//“a” per arena statistics, respectively; 
//“b” per size class statistics for bins
//“l” per size class statistics for large objects, respectively; 
//“x” can be specified to omit all mutex statistics;
//“e” can be used to omit extent statistics. 
void malloc_stats_print(void (*write_cb) (void *, const char *), void *cbopaque, const char *opts);

//内存空间可用大小
size_t malloc_usable_size(const void *ptr);

void (*malloc_message)(void *cbopaque, const char *s);

const char *malloc_conf;

