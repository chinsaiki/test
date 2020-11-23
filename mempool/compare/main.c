#include <stdio.h>

#include "compare.h"


inline void sys_glibc_malloc_init(void*ptr);
inline void* sys_glibc_malloc(size_t size);
inline void sys_glibc_test(void*ptr, size_t size);
inline void sys_glibc_free(void*ptr);


inline void obstack_malloc_init(void*ptr);
inline void* obstack_malloc_malloc(size_t size);
inline void obstack_malloc_test(void*ptr, size_t size);
inline void obstack_malloc_free(void*ptr);



inline void nmx_malloc_init(void*ptr);
inline void* nmx_malloc_malloc(size_t size);
inline void nmx_malloc_test(void*ptr, size_t size);
inline void nmx_malloc_free(void*ptr);



inline void ncx_malloc_init(void*ptr);
inline void* ncx_malloc_malloc(size_t size);
inline void ncx_malloc_test(void*ptr, size_t size);
inline void ncx_malloc_free(void*ptr);


inline void redis_zmalloc_init(void*ptr);
inline void* redis_zmalloc_malloc(size_t size);
inline void redis_zmalloc_test(void*ptr, size_t size);
inline void redis_zmalloc_free(void*ptr);




int main(int argc, char *argv[])
{
    unsigned long int cnt = 0;
    switch(argc){
        case 3:
            cnt = strtoul(argv[2], NULL, 0);
            set_alloc_size(cnt?cnt:get_alloc_size());

        case 2:
            cnt = strtoul(argv[1], NULL, 0);
            set_alloc_free_cnt(cnt?cnt:get_alloc_free_cnt());

        case 1:
        default:
            printf("Test mem size %u, loop times %u.\n", get_alloc_size(), get_alloc_free_cnt());
            break;
    }   

    struct malloc_entity sys_memory_alloc;
    struct malloc_entity obstack_memory_alloc;
    struct malloc_entity ncx_alloc;
    struct malloc_entity redis_zmalloc;

    /* 初始化测试实例 */
    malloc_entity_init(&sys_memory_alloc, "Glibc", 
            sys_glibc_malloc_init, sys_glibc_malloc, sys_glibc_test, sys_glibc_free);
    malloc_entity_init(&obstack_memory_alloc, "Obstack", 
            obstack_malloc_init, obstack_malloc_malloc, obstack_malloc_test, obstack_malloc_free);
    malloc_entity_init(&ncx_alloc, "NCX", 
            ncx_malloc_init, ncx_malloc_malloc, ncx_malloc_test, ncx_malloc_free);
    malloc_entity_init(&redis_zmalloc, "Redis", 
            redis_zmalloc_init, redis_zmalloc_malloc, redis_zmalloc_test, redis_zmalloc_free);

    /* 进行测试 */
    printf("TEST: sys_memory_alloc\n");
    malloc_entity_test(&sys_memory_alloc);
    printf("TEST: obstack_memory_alloc\n");
    malloc_entity_test(&obstack_memory_alloc);
    printf("TEST: ncx_alloc\n");
    malloc_entity_test(&ncx_alloc);
    printf("TEST: Redis_zmalloc\n");
    malloc_entity_test(&redis_zmalloc);

    /* 打印测试信息 */
    malloc_entity_display(&sys_memory_alloc);
    malloc_entity_display(&obstack_memory_alloc);
    malloc_entity_display(&ncx_alloc);
    malloc_entity_display(&redis_zmalloc);
}


//             Total      Alloc       Test       Free
//  Glibc     442025       1900     438505       1620
//Obstack     449411        714     448416        281
//    NMX     448694       6609     440822       1263
//    NCX     447031       4455     440057       2519

