#include <malloc.h>
#include <stdio.h>

int main()
{
    char *str = malloc(64);

    int i;
#ifdef LEAK    
    for (i=0; i<10; i++) {
        str = malloc(64);
    }
#endif //LEAK    
    free(str);
    printf("Exit program.\n");
}
