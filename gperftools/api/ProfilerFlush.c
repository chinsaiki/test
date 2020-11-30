#include <stdio.h>
#include <gperftools/tcmalloc.h>
#include <gperftools/profiler.h>

/* Flush any currently buffered profiling state to the profile file.
 * Has no effect if the profiler has not been started.
 */
PERFTOOLS_DLL_DECL void ProfilerFlush(void);


int main()
{
    ProfilerStart("rtoax");
    
    char *str = tc_malloc(1024);
    printf("%s\n", str);
    

    tc_free(str);

    ProfilerFlush();
    
    ProfilerStop();
}


