#include <ucontext.h>

//getcontext, setcontext - get or set the user context
int getcontext(ucontext_t *ucp);
int setcontext(const ucontext_t *ucp);


