#include <dlfcn.h>

/* These are the possible values for the FLAGS argument to `dladdr1'.
   This indicates what extra information is stored at *EXTRA_INFO.
   It may also be zero, in which case the EXTRA_INFO argument is not used.  */
enum
  {
    /* Matching symbol table entry (const ElfNN_Sym *).  */
    RTLD_DL_SYMENT = 1,

    /* The object containing the address (struct link_map *).  */
    RTLD_DL_LINKMAP = 2
  };


/* Fill in *INFO with the following information about ADDRESS.
   Returns 0 iff no shared object's segments contain that address.  */
int dladdr (const void *__address, Dl_info *__info);

/* Same as `dladdr', but additionally sets *EXTRA_INFO according to FLAGS.  */
int dladdr1 (const void *__address, Dl_info *__info,
		    void **__extra_info, int __flags);

