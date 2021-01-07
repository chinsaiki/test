#include <ucontext.h>

//makecontext, swapcontext - manipulate user context
void makecontext(ucontext_t *ucp, void (*func)(), int argc, ...);

int swapcontext(ucontext_t *oucp, ucontext_t *ucp);

