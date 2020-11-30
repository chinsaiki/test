#include <stdio.h>
#include <gperftools/tcmalloc.h>
#include <gperftools/profiler.h>

/* Stop profiling. Can be started again with ProfilerStart(), but
 * the currently accumulated profiling data will be cleared.
 */
PERFTOOLS_DLL_DECL void ProfilerStop(void);



int main()
{
    ProfilerStart("rtoax");
    
    char *str = tc_malloc(1024);
    printf("%s\n", str);
    

    tc_free(str);

    ProfilerFlush();
    
    ProfilerStop();
}

