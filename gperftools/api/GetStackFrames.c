#include <stdio.h>
#include <gperftools/tcmalloc.h>
#include <gperftools/stacktrace.h>

void* tc_malloc(size_t size);
void tc_free(void* ptr);

// Skips the most recent "skip_count" stack frames (also skips the
// frame generated for the "GetStackFrames" routine itself), and then
// records the pc values for up to the next "max_depth" frames in
// "result", and the corresponding stack frame sizes in "sizes".
// Returns the number of values recorded in "result"/"sizes".
//
// Example:
//      main() { foo(); }
//      foo() { bar(); }
//      bar() {
//        void* result[10];
//        int sizes[10];
//        int depth = GetStackFrames(result, sizes, 10, 1);
//      }
//
// The GetStackFrames call will skip the frame for "bar".  It will
// return 2 and will produce pc values that map to the following
// procedures:
//      result[0]       foo
//      result[1]       main
// (Actually, there may be a few more entries after "main" to account for
// startup procedures.)
// And corresponding stack frame sizes will also be recorded:
//    sizes[0]       16
//    sizes[1]       16
// (Stack frame sizes of 16 above are just for illustration purposes.)
// Stack frame sizes of 0 or less indicate that those frame sizes couldn't
// be identified.
//
// This routine may return fewer stack frame entries than are
// available. Also note that "result" and "sizes" must both be non-NULL.
extern PERFTOOLS_DLL_DECL int GetStackFrames(void** result, int* sizes, int max_depth,
                          int skip_count);


int main()
{
    char *str = tc_malloc(1024);
    printf("%s\n", str);

    void* result[10];
    int sizes[10];
    int depth = GetStackFrames(result, sizes, 10, 1);
    
    printf("depth = %d\n", depth);

    tc_free(str);
}

