/* Provide a real file - not a symlink - as it would cause multiarch conflicts
   when multiple different arch releases are installed simultaneously.  */

/* Fedora/RHEL does not cross-compile libunwind, so always use direct
 * inclusion.  It handles multilib better.
 */
#ifndef LIBUNWIND_INCLUDE_H
#define LIBUNWIND_INCLUDE_H 1

#if defined __aarch64__
#include "libunwind-aarch64.h"
#elif defined __arm__
# include "libunwind-arm.h"
#elif defined __hppa__
# include "libunwind-hppa.h"
#elif defined __ia64__
# include "libunwind-ia64.h"
#elif defined __mips__
# include "libunwind-mips.h"
#elif defined __powerpc__ && !defined __powerpc64__
# include "libunwind-ppc32.h"
#elif defined __powerpc64__
# include "libunwind-ppc64.h"
#elif defined __sh__
# include "libunwind-sh.h"
#elif defined __i386__
# include "libunwind-x86.h"
#elif defined __x86_64__
# include "libunwind-x86_64.h"
#elif defined __tilegx__
# include "libunwind-tilegx.h"
#else
# error "Unsupported arch"
#endif
#endif /* LIBUNWIND_INCLUDE_H */
