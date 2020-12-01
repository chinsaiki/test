#include <pthread.h>
#include <stdio.h>

/* Return the thread descriptor for the current thread.  */
# define THREAD_SELF \
	 ((char *)__builtin_thread_pointer () - 1)

pthread_t rtoax_pthread_self (void)
{
	  return (pthread_t) THREAD_SELF;
}

int main()
{
	printf("%ld:%ld\n", rtoax_pthread_self(), pthread_self());
}
