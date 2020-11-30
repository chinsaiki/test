#include <stdio.h>
#include <gperftools/tcmalloc.h>
#include <gperftools/heap-profiler.h>

void* tc_malloc(size_t size);
void tc_free(void* ptr);
/* Generate current heap profiling information.
 * Returns an empty string when heap profiling is not active.
 * The returned pointer is a '\0'-terminated string allocated using malloc()
 * and should be free()-ed as soon as the caller does not need it anymore.
 */
PERFTOOLS_DLL_DECL char* GetHeapProfile();


int main()
{
    HeapProfilerStart("rtoax");

    char *str = tc_malloc(1024);
    printf("%s\n", str);

    int ret = MallocExtension_VerifyAllMemory();
    printf("%d\n", ret);

    
    printf("GetHeapProfile() \n %s\n", GetHeapProfile());
    
    HeapProfilerStop();

    tc_free(str);

}
