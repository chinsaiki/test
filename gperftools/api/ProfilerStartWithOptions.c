#include <stdio.h>
#include <string.h>
#include <gperftools/tcmalloc.h>
#include <gperftools/profiler.h>

/* Start profiling and write profile into fname, discarding any
 * existing profiling data in that file.
 *
 * The profiler is configured using the options given by 'options'.
 * Options which are not specified are given default values.
 *
 * 'options' may be NULL, in which case all are given default values.
 *
 * Returns nonzero if profiling was started successfully, or zero else.
 */
PERFTOOLS_DLL_DECL int ProfilerStartWithOptions(
    const char *fname, const struct ProfilerOptions *options);

int my_filter_in_thread(void *arg)
{
    char *hello = (char*)arg;
    printf("%s\n", hello);

    return -1;
}

void print_ProfilerState(struct ProfilerState *state)
{
    printf("%s\n", state->enabled?"enable":"disable");
    printf("start time %ld (use %ld)\n", state->start_time, time(NULL) - state->start_time);
    printf("profile %s\n", state->profile_name);
    printf("samples_gathered %d\n", state->samples_gathered);
}

//struct ProfilerState {
//  int    enabled;             /* Is profiling currently enabled? */
//  time_t start_time;          /* If enabled, when was profiling started? */
//  char   profile_name[1024];  /* Name of profile file being written, or '\0' */
//  int    samples_gathered;    /* Number of samples gathered so far (or 0) */
//};


static char helloA[] = "HelloA";

struct ProfilerOptions myoption;
struct ProfilerState state;

int main()
{
    memset(&myoption, 0, sizeof myoption);
    
    myoption.filter_in_thread = &my_filter_in_thread;
    myoption.filter_in_thread_arg = helloA;
    
    ProfilerStartWithOptions("rtoax-profiler.txt.out", &myoption);

    ProfilerRegisterThread();

    ProfilerEnable();
    
    ProfilerFlush();
    
    char *str = tc_malloc(1024);
    printf("%s\n", str);

    tc_free(str);

    sleep(1);


    ProfilerGetCurrentState(&state);
    print_ProfilerState(&state);
    
    ProfilerFlush();
    
    ProfilerStop();
}

