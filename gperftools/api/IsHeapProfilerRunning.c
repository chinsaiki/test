#include <stdio.h>
#include <gperftools/tcmalloc.h>
#include <gperftools/heap-profiler.h>

void* tc_malloc(size_t size);
void tc_free(void* ptr);
void HeapProfilerStart(const char* prefix);
/* Returns non-zero if we are currently profiling the heap.  (Returns
 * an int rather than a bool so it's usable from C.)  This is true
 * between calls to HeapProfilerStart() and HeapProfilerStop(), and
 * also if the program has been run with HEAPPROFILER, or some other
 * way to turn on whole-program profiling.
 */
int IsHeapProfilerRunning();


int main()
{
    HeapProfilerStart("rtoax");

    char *str = tc_malloc(1024);
    printf("%s\n", str);

    int ret = MallocExtension_VerifyAllMemory();
    printf("%d\n", ret);

    ret = IsHeapProfilerRunning();
    printf("%d\n", ret);

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


