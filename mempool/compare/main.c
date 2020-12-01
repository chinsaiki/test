#include <stdio.h>
#include <stdlib.h>
#include "compare.h"

#ifdef PTMALLOC
inline void sys_glibc_malloc_init(void*user_ptr);
inline void* sys_glibc_malloc(void*user_ptr, size_t size);
inline void sys_glibc_test(void*user_ptr, void*ptr, size_t size);
inline void sys_glibc_free(void*user_ptr, void*ptr);
#endif

#ifdef OBSTACK
inline void obstack_malloc_init(void*user_ptr);
inline void* obstack_malloc_malloc(void*user_ptr, size_t size);
inline void obstack_malloc_test(void*user_ptr, void*ptr, size_t size);
inline void obstack_malloc_free(void*user_ptr, void*ptr);
#endif

#ifdef NGINX
inline void ncx_malloc_init(void*user_ptr);
inline void* ncx_malloc_malloc(void*user_ptr, size_t size);
inline void ncx_malloc_test(void*user_ptr, void*ptr, size_t size);
inline void ncx_malloc_free(void*user_ptr, void*ptr);
#endif

#ifdef REDIS
inline void redis_zmalloc_init(void*user_ptr);
inline void* redis_zmalloc_malloc(void*user_ptr, size_t size);
inline void redis_zmalloc_test(void*user_ptr, void*ptr, size_t size);
inline void redis_zmalloc_free(void*user_ptr, void*ptr);
#endif

#ifdef JEMALLOC
inline void jemalloc_malloc_init(void*user_ptr);
inline void* jemalloc_malloc(void*user_ptr, size_t size);
inline void jemalloc_test(void*user_ptr, void*ptr, size_t size);
inline void jemalloc_free(void*user_ptr, void*ptr);
#endif

#ifdef TCMALLOC
inline void tcmalloc_malloc_init(void*user_ptr);
inline void* tcmalloc_malloc(void*user_ptr, size_t size);
inline void tcmalloc_test(void*user_ptr, void*ptr, size_t size);
inline void tcmalloc_free(void*user_ptr, void*ptr);
#endif

#ifdef LIBVMEM
inline void vmem_malloc_init(void*user_ptr);
inline void* vmem_malloc_malloc(void*user_ptr, size_t size);
inline void vmem_malloc_test(void*user_ptr, void*ptr, size_t size);
inline void vmem_malloc_free(void*user_ptr, void*ptr);
#endif

int main(int argc, char *argv[])
{
    unsigned long int cnt = 0;
    switch(argc){
        case 4:
            cnt = atoi(argv[3]);
            set_nr_threads(cnt?cnt:get_nr_threads());
            if(get_nr_threads() > NR_THREAD) {
                printf("Threads must less than %d\n", NR_THREAD);
                exit(1);
            }
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
#ifdef PTMALLOC
    struct malloc_entity sys_memory_alloc;
#endif
#ifdef OBSTACK
    struct malloc_entity obstack_memory_alloc;
#endif
    struct malloc_entity ncx_alloc;
#ifdef REDIS
    struct malloc_entity redis_zmalloc;
#endif
#ifdef JEMALLOC
    struct malloc_entity jemalloc_alloc;
#endif
#ifdef TCMALLOC
    struct malloc_entity tcmalloc_alloc;
#endif
#ifdef LIBVMEM
    struct malloc_entity vmem_alloc;
#endif

    /* 初始化测试实例 */
#ifdef PTMALLOC
    malloc_entity_init(&sys_memory_alloc, "Glibc", 
            sys_glibc_malloc_init, sys_glibc_malloc, sys_glibc_test, sys_glibc_free, NULL);
#endif
#ifdef OBSTACK
    malloc_entity_init(&obstack_memory_alloc, "Obstack", 
            obstack_malloc_init, obstack_malloc_malloc, obstack_malloc_test, obstack_malloc_free, NULL);
#endif
#ifdef NGINX
    malloc_entity_init(&ncx_alloc, "NCX", 
            ncx_malloc_init, ncx_malloc_malloc, ncx_malloc_test, ncx_malloc_free, NULL);
#endif
#ifdef REDIS
    malloc_entity_init(&redis_zmalloc, "Redis", 
            redis_zmalloc_init, redis_zmalloc_malloc, redis_zmalloc_test, redis_zmalloc_free, NULL);
#endif
#ifdef JEMALLOC
    malloc_entity_init(&jemalloc_alloc, "jemalloc", 
            jemalloc_malloc_init, jemalloc_malloc, jemalloc_test, jemalloc_free, NULL);
#endif
#ifdef TCMALLOC
    malloc_entity_init(&tcmalloc_alloc, "tcmalloc", 
            tcmalloc_malloc_init, tcmalloc_malloc, tcmalloc_test, tcmalloc_free, NULL);
#endif
#ifdef LIBVMEM
    malloc_entity_init(&vmem_alloc, "vmem", 
            vmem_malloc_init, vmem_malloc_malloc, vmem_malloc_test, vmem_malloc_free, NULL);
#endif


    /* 进行测试 */
#ifdef PTMALLOC
    debug_print("TEST: sys_memory_alloc\n");
    malloc_entity_test(&sys_memory_alloc);
#endif    
#ifdef OBSTACK
    debug_print("TEST: obstack_memory_alloc\n");
    malloc_entity_test(&obstack_memory_alloc);
#endif    
#ifdef NGINX
    debug_print("TEST: ncx_alloc\n");
    malloc_entity_test(&ncx_alloc);
#endif    
#ifdef REDIS
    debug_print("TEST: Redis_zmalloc\n");
    malloc_entity_test(&redis_zmalloc);
#endif    
#ifdef JEMALLOC
    debug_print("TEST: jemalloc_alloc\n");
    malloc_entity_test(&jemalloc_alloc);
#endif    
#ifdef TCMALLOC
    debug_print("TEST: tcmalloc_alloc\n");
    malloc_entity_test(&tcmalloc_alloc);
#endif
#ifdef LIBVMEM
    debug_print("TEST: vmem_alloc\n");
    malloc_entity_test(&vmem_alloc);
#endif

    /* 打印测试信息 */
#ifdef PTMALLOC
    malloc_entity_display(&sys_memory_alloc);
#endif
#ifdef OBSTACK
    malloc_entity_display(&obstack_memory_alloc);
#endif
#ifdef NGINX
    malloc_entity_display(&ncx_alloc);
#endif
#ifdef REDIS
    malloc_entity_display(&redis_zmalloc);
#endif
#ifdef JEMALLOC
    malloc_entity_display(&jemalloc_alloc);
#endif
#ifdef TCMALLOC
    malloc_entity_display(&tcmalloc_alloc);
#endif
#ifdef LIBVMEM
    malloc_entity_display(&vmem_alloc);
#endif
    
    exit(1);
}


//             Total      Alloc       Test       Free
//  Glibc     442025       1900     438505       1620
//Obstack     449411        714     448416        281
//    NMX     448694       6609     440822       1263
//    NCX     447031       4455     440057       2519

