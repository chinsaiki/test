#include <pthread.h>
#include <stdio.h>

#if 0
register void *__thread_self asm ("tp");

#define READ_THREAD_POINTER() ({ __thread_self; })

#elif 0

/* Define r31 as thread pointer register.  */
#define READ_THREAD_POINTER() \
  ({ void *__result;						\
     __asm__ __volatile__ ("mov %0, r31"			\
                           : "=r" (__result));			\
     __result; })
#elif 0
#define READ_THREAD_POINTER() \
     ({ void *__result;                               \
        asm volatile ("rdhwr\t%0, $29" : "=v" (__result));                \
        __result; })
#elif 1
#define READ_THREAD_POINTER() \
     ({ void *__result;							      \
        asm volatile (".set\tpush\n\t.set\tmips32r2\n\t"			      \
		      "rdhwr\t%0, $29\n\t.set\tpop" : "=v" (__result));	      \
        __result; })

#endif

pthread_t rtoax_pthread_self (void)
{
	  return (pthread_t) READ_THREAD_POINTER();
}

int main()
{
	printf("%ld:%ld\n", rtoax_pthread_self(), pthread_self());
}

