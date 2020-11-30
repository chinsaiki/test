#include <stdio.h>
#include <gperftools/tcmalloc.h>
#include <gperftools/heap-profiler.h>

void* tc_malloc(size_t size);
void tc_free(void* ptr);
/* Stop heap profiling.  Can be restarted again with HeapProfilerStart(),
 * but the currently accumulated profiling information will be cleared.
 */
PERFTOOLS_DLL_DECL void HeapProfilerStop();


int main()
{
    HeapProfilerStart("rtoax");

    char *str = tc_malloc(1024);
    printf("%s\n", str);

    int ret = MallocExtension_VerifyAllMemory();
    printf("%d\n", ret);
    
    HeapProfilerStop();

    tc_free(str);

}

//[root@localhost api]# ./compile.sh HeapProfilerStart.c 
//[root@localhost api]# ./a.out 
//Starting tracking the heap
//
//1
//Dumping heap profile to rtoax.0001.heap (Exiting, 0 bytes in use)
//[root@localhost api]# ./compile.sh HeapProfilerStart.c 
//[root@localhost api]# ./a.out 
//Starting tracking the heap
//
//1
//Dumping heap profile to rtoax.0001.heap (Exiting, 1 kB in use)


