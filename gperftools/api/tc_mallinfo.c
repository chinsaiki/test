#include <gperftools/tcmalloc.h>
#include <stdio.h>

//void* tc_malloc(size_t size);
//void tc_free(void* ptr);
//void* tc_valloc(size_t __size);
//void* tc_pvalloc(size_t __size);
//void tc_malloc_stats(void);
//int tc_mallopt(int cmd, int value);
struct mallinfo tc_mallinfo(void);

//struct mallinfo {
//   int arena;     /* Non-mmapped space allocated (bytes) */
//   int ordblks;   /* Number of free chunks */
//   int smblks;    /* Number of free fastbin blocks */
//   int hblks;     /* Number of mmapped regions */
//   int hblkhd;    /* Space allocated in mmapped regions (bytes) */
//   int usmblks;   /* Maximum total allocated space (bytes) */
//   int fsmblks;   /* Space in freed fastbin blocks (bytes) */
//   int uordblks;  /* Total allocated space (bytes) */
//   int fordblks;  /* Total free space (bytes) */
//   int keepcost;  /* Top-most, releasable space (bytes) */
//};

static void print_mallinfo(struct mallinfo *info){

    struct mallinfo *mf1 = info;
    printf("------------------------------------------\n");
    printf("mallinfo.arena      = %d.\n", mf1->arena);
    printf("mallinfo.ordblks    = %d.\n", mf1->ordblks);
    printf("mallinfo.smblks     = %d.\n", mf1->smblks);
    printf("mallinfo.hblks      = %d.\n", mf1->hblks);
    printf("mallinfo.hblkhd     = %d.\n", mf1->hblkhd);
    printf("mallinfo.usmblks    = %d.\n", mf1->usmblks);
    printf("mallinfo.fsmblks    = %d.\n", mf1->fsmblks);
    printf("mallinfo.uordblks   = %d.\n", mf1->uordblks);
    printf("mallinfo.fordblks   = %d.\n", mf1->fordblks);
    printf("mallinfo.keepcost   = %d.\n", mf1->keepcost);
}

int main()
{
    struct mallinfo info = tc_mallinfo();
    print_mallinfo(&info);
    
    char *str = tc_pvalloc(1024*1024*1024);
    printf("%s\n", str);
    
//    tc_malloc_stats();

    info = tc_mallinfo();

    print_mallinfo(&info);

    tc_free(str);

    info = tc_mallinfo();
    print_mallinfo(&info);
}



