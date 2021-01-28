#include <strings.h>
#include <xlocale.h>

/* Again versions of a few functions which use the given locale instead
   of the global one.  */
extern int strcasecmp_l (const char *__s1, const char *__s2, __locale_t __loc);

extern int strncasecmp_l (const char *__s1, const char *__s2,
			  size_t __n, __locale_t __loc);

