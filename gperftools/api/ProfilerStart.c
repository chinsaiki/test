#include <stdio.h>
#include <gperftools/tcmalloc.h>
#include <gperftools/profiler.h>

/* Start profiling and write profile info into fname, discarding any
 * existing profiling data in that file.
 *
 * This is equivalent to calling ProfilerStartWithOptions(fname, NULL).
 */
PERFTOOLS_DLL_DECL int ProfilerStart(const char* fname);

int main()
{
    ProfilerStart("rtoax");
    
    char *str = tc_malloc(1024);
    printf("%s\n", str);
    

    tc_free(str);

    ProfilerFlush();
    
    ProfilerStop();
}
