#include <netdb.h>


/* Test whether NETGROUP contains the triple (HOST,USER,DOMAIN).

   This function is not part of POSIX and therefore no official
   cancellation point.  But due to similarity with an POSIX interface
   or due to the implementation it is a cancellation point and
   therefore not marked with __THROW.  */
extern int innetgr (const char *__netgroup, const char *__host,
		    const char *__user, const char *__domain);

