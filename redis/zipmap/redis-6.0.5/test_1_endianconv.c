#include "endianconv.h"
#include <stdio.h>


#include <stdio.h>

#define UNUSED(x) (void)(x)
int endianconvTest(int argc, char *argv[]) {
    char buf[32];

    UNUSED(argc);
    UNUSED(argv);

    sprintf(buf,"ciaoroma");
    memrev16(buf);
    printf("%s\n", buf);

    sprintf(buf,"ciaoroma");
    memrev32(buf);
    printf("%s\n", buf);

    sprintf(buf,"ciaoroma");
    memrev64(buf);
    printf("%s\n", buf);

    return 0;
}


int main(int argc, char **argv)
{
    endianconvTest(argc, argv);


}

