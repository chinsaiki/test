#include "sha256.h"
#include <stdio.h>


int main()
{
    char str[] = {"abcdefghijklmn"};
    BYTE hash[1024];
    
    SHA256_CTX ctx;
    sha256_init(&ctx);

    sha256_update(&ctx, str, strlen(str));

    sha256_final(&ctx, hash);

    char *s_hash = (char*)hash;

    printf("%s -> sha256: %s\n", str, s_hash);
}

