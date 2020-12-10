#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <obstack.h>

#if 1
#define debug()
#else
#define debug() printf("%s:%d\n",__func__, __LINE__)
#endif


void *mymalloc(ssize_t size){
    debug();
    return malloc(size);
}
void myfree(void*ptr){
    debug();
    free(ptr);
}



#define obstack_chunk_alloc mymalloc
#define obstack_chunk_free myfree
#define obstack_alloc_failed_handler  TODO

char * copystring(struct obstack *obj, char *string)
{
    size_t len = strlen (string) + 1;
    char *s = (char*) obstack_alloc (obj, len);
    memcpy (s, string , len);
    return s;
}

int main()
{
    struct obstack string_obstack;
    obstack_init(&string_obstack);

    printf("obstack_chunk_size = %d\n", obstack_chunk_size(&string_obstack));

    char *str1 = copystring(&string_obstack, "rongtao");
    printf("str1 = %s\n", str1);
    char *str2 = copystring(&string_obstack, "HelloWorld");
    printf("str2 = %s\n", str2);

    obstack_free(&string_obstack, str1);
    obstack_free(&string_obstack, str2);
    
    char *str3 = (char*) obstack_alloc (&string_obstack, obstack_chunk_size(&string_obstack));
    printf("str3 = %p\n", str3);
    
    obstack_free(&string_obstack, str3);

    obstack_finish(&string_obstack);
}


