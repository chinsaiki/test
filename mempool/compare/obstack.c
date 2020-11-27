#ifdef OBSTACK

#include <malloc.h>
#include <obstack.h>

#include "compare.h"



#define obstack_chunk_alloc __mymalloc
#define obstack_chunk_free __myfree
#define obstack_alloc_failed_handler  TODO


static struct obstack crypto_obstack;


static void *__mymalloc(ssize_t size){  debug(); return malloc(size);}
static void __myfree(void*ptr){  debug(); free(ptr);}



inline void obstack_malloc_init(void*ptr)
{
    obstack_init(&crypto_obstack);

}

inline void* obstack_malloc_malloc(void *user_ptr, size_t size)
{
    return obstack_alloc(&crypto_obstack, size);
}
inline void obstack_malloc_test(void *user_ptr, void*ptr, size_t size)
{
    malloc_common_test(ptr, size);
}
inline void obstack_malloc_free(void *user_ptr, void*ptr)
{
    obstack_free(&crypto_obstack, ptr);
}

#endif
