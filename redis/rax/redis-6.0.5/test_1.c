#include "rax.h"
#include <stdio.h>

struct user_info{
    char name[16];
    int age;
    int sex;
};

#define RTOAX1   "rtoax1"
#define RTOAX2   "rtoax2"
#define RTOAX3   "rtoax3"

struct user_info rtoax1 = {
    .name = RTOAX1,
    .age = 18,
    .sex = 1,
};
    
struct user_info rtoax2 = {
    .name = RTOAX2,
    .age = 18,
    .sex = 1,
};
        
struct user_info rtoax3 = {
    .name = RTOAX3,
    .age = 18,
    .sex = 1,
};


int main(int argc, char **argv)
{
    struct user_info *info = NULL;

    rax *rax1 = raxNew();

    raxInsert(rax1, RTOAX1, sizeof(RTOAX1), &rtoax1, NULL);
    raxInsert(rax1, RTOAX2, sizeof(RTOAX2), &rtoax2, NULL);
    raxInsert(rax1, RTOAX3, sizeof(RTOAX3), &rtoax3, NULL);

    info = (struct user_info *)raxFind(rax1, RTOAX2, sizeof(RTOAX2));
    printf("%s, %d, %d.\n", info->name, info->age, info->sex);

    raxFree(rax1);
}

