#include <malloc.h>
#include <stdio.h>

int main()
{
    char *str_pad1 = malloc(64);
    char *str = malloc(64);
    char *str_pad2 = malloc(64);

#ifdef CLOBBER
    str[-1] = 'A';
    str[64] = 'A';
#else
    str[0] = 'A';
    str[63] = 'A';
#endif
    
    free(str);
    free(str_pad1);
    free(str_pad2);
    printf("Exit program.\n");
}

