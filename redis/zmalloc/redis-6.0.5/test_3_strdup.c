#include <string.h>
#include <sys/time.h>

#include "zmalloc.h"

static unsigned long int diff_timeval(const char *prefix, struct timeval *big, struct timeval *small){
    unsigned long int diffsec = big->tv_sec - small->tv_sec;
    unsigned long int diffusec = diffsec*1000000 + (big->tv_usec - small->tv_usec);
    printf("%15s %ld MicroSec\n", prefix, diffusec);
    return diffusec;
}

static const char STR[] = {
"STRDUP(3)"\
"NAME"\
"       strdup, strndup, strdupa, strndupa - duplicate a string"\
""\
"SYNOPSIS"\
"       #include <string.h>"\
"       char *strdup(const char *s);"\
"       char *strndup(const char *s, size_t n);"\
"       char *strdupa(const char *s);"\
"       char *strndupa(const char *s, size_t n);"
};

unsigned long int test_alloc_free_cnt = 10000;

void test_str(void*mem){
//    memset(mem, 0, size);
}

void test_strdup()
{
    struct timeval start, end;
    char *str = NULL;
    
    gettimeofday(&start, NULL);
    unsigned long int i;
    for(i=0;i<test_alloc_free_cnt;i++) {
        str = strdup(STR);
        test_str(str);
        free(str);
    }
    
    gettimeofday(&end, NULL);

    diff_timeval(" strdup>>", &end, &start);
}

void test_vstrdup()
{
    struct timeval start, end;
    char *str = NULL;
    
    gettimeofday(&start, NULL);
    unsigned long int i;
    for(i=0;i<test_alloc_free_cnt;i++) {
        str = zstrdup(STR);
        test_str(str);
        zfree(str);
    }
    
    gettimeofday(&end, NULL);

    diff_timeval("zstrdup>>", &end, &start);
}


int main(int argc, char **argv)
{
    unsigned long int cnt = 0;
    switch(argc){
        case 2:
            cnt = strtoul(argv[1], NULL, 0);
            test_alloc_free_cnt = cnt?cnt:test_alloc_free_cnt;

        case 1:
        default:
            printf("Test mem loop times %u.\n", test_alloc_free_cnt);
            break;
    }
    
    test_strdup();
    test_vstrdup();
}

//结论；redis的strdup性能与glibc的基本相同
//     strdup>> 554461 MicroSec
//    zstrdup>> 588182 MicroSec

