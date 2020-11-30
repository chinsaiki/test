#include <stdio.h>
#include <gperftools/tcmalloc.h>
#include <gperftools/stacktrace.h>

void* tc_malloc(size_t size);
void tc_free(void* ptr);

// This is similar to the GetStackFrames routine, except that it returns
// the stack trace only, and not the stack frame sizes as well.
// Example:
//      main() { foo(); }
//      foo() { bar(); }
//      bar() {
//        void* result[10];
//        int depth = GetStackTrace(result, 10, 1);
//      }
//
// This produces:
//      result[0]       foo
//      result[1]       main
//           ....       ...
//
// "result" must not be NULL.
extern PERFTOOLS_DLL_DECL int GetStackTrace(void** result, int max_depth,
                                            int skip_count);


int main()
{
    char *str = tc_malloc(1024);
    printf("%s\n", str);

    void* result[10];
    int depth = GetStackTrace(result, 10, 1);
    
    printf("depth = %d\n", depth);

    tc_free(str);
}


