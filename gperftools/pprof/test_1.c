#include <stdio.h>
#include <string.h>
#include <gperftools/tcmalloc.h>
#include <gperftools/profiler.h>

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


static char helloA[] = "HelloA";

struct ProfilerOptions myoption;
struct ProfilerState state;


void test_profile1()
{
    long int i;
    char *str;
    for(i=0;i<1000000; i++){
        str = tc_malloc(1024);
        tc_free(str);
    }
}

int main()
{
    memset(&myoption, 0, sizeof myoption);
    
    myoption.filter_in_thread = &my_filter_in_thread;
    myoption.filter_in_thread_arg = helloA;
    
    ProfilerStartWithOptions("rtoax-profiler.txt.out", &myoption);

    ProfilerRegisterThread();

    ProfilerEnable();
    
    ProfilerFlush();
    
    test_profile1();

    sleep(1);


    ProfilerGetCurrentState(&state);
    print_ProfilerState(&state);
    
    ProfilerFlush();
    
    ProfilerStop();
}


