#include <stdio.h>
#include <gperftools/tcmalloc.h>
#include <gperftools/heap-profiler.h>

void* tc_malloc(size_t size);
void tc_free(void* ptr);
/* Dump a profile now - can be used for dumping at a hopefully
 * quiescent state in your program, in order to more easily track down
 * memory leaks. Will include the reason in the logged message
 */
PERFTOOLS_DLL_DECL void HeapProfilerDump(const char *reason);


int main()
{
    HeapProfilerStart("rtoax");

    char *str = tc_malloc(1024);
    printf("%s\n", str);

    int ret = MallocExtension_VerifyAllMemory();
    printf("%d\n", ret);

    HeapProfilerDump("--------------------I just wanna to dump.----------------");
    
    HeapProfilerStop();



    HeapProfilerDump("--------------------I just wanna to dump 2.----------------");


    tc_free(str);

}

