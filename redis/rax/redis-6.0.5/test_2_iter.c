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


//void raxStart(raxIterator *it, rax *rt);
//int raxSeek(raxIterator *it, const char *op, unsigned char *ele, size_t len);
//int raxNext(raxIterator *it);
//int raxPrev(raxIterator *it);
//int raxRandomWalk(raxIterator *it, size_t steps);
//int raxCompare(raxIterator *iter, const char *op, unsigned char *key, size_t key_len);
//void raxStop(raxIterator *it);
//int raxEOF(raxIterator *it);

int main(int argc, char **argv)
{
    struct user_info *info = NULL;

    raxIterator iter;

    rax *rax1 = raxNew();

    raxInsert(rax1, RTOAX1, sizeof(RTOAX1), &rtoax1, NULL);
    raxInsert(rax1, RTOAX2, sizeof(RTOAX2), &rtoax2, NULL);
    raxInsert(rax1, RTOAX3, sizeof(RTOAX3), &rtoax3, NULL);

    info = (struct user_info *)raxFind(rax1, RTOAX2, sizeof(RTOAX2));
    printf("%s, %d, %d.\n", info->name, info->age, info->sex);

    /* test1 从第一个遍历 */
    printf("---------------------.\n");
    raxStart(&iter, rax1);
    raxSeek(&iter,"^",NULL,0);
    while(raxNext(&iter)) {
        info = iter.data;
        printf("Iteration: %s, %d, %d.\n", info->name, info->age, info->sex);
    }
    raxStop(&iter);

    /* test2 从最后一个遍历 */
    printf("---------------------.\n");
    raxStart(&iter, rax1);
    raxSeek(&iter,"$",NULL,0);
    while(raxPrev(&iter)) {
        info = iter.data;
        printf("Iteration: %s, %d, %d.\n", info->name, info->age, info->sex);
    }
    raxStop(&iter);

    
    /* test3 >=遍历 */
//    printf("---------------------.\n");
//    raxStart(&iter, rax1);
//    raxSeek(&iter,"=",&rtoax3,sizeof(struct user_info));
//    printf("Seek : %s, %d, %d.\n", rtoax3.name, rtoax3.age, rtoax3.sex);
//    while(raxNext(&iter)) {
//        info = iter.data;
//        printf("Iteration: %s, %d, %d.\n", info->name, info->age, info->sex);
//    }
//    raxStop(&iter);

    raxFree(rax1);
}


