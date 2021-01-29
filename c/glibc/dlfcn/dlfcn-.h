/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 06月 14日 星期五 09:05:10 CST. */
/* 
 Copyright (C) Rong Tao @Beijing

 Permission is granted to copy, distribute and/or modify this document
 under the terms of the GNU Free Documentation License, Version 1.3
 or any later version published by the Free Software Foundation;
 with no Invariant Sections, no Front-Cover Texts, and no Back-Cover
 Texts. A copy of the license is included in the section entitled ‘‘GNU
 Free Documentation License’’.
   2019年 03月 14日 星期四 19:24:23 CST. 
*/
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 14日 星期四 17:04:49 CST. */

#include <dlfcn.h>


#ifdef __USE_GNU
/* If the first argument of `dlsym' or `dlvsym' is set to RTLD_NEXT
   the run-time address of the symbol called NAME in the next shared
   object is returned.  The "next" relation is defined by the order
   the shared objects were loaded.  */
# define RTLD_NEXT	((void *) -1l)

/* If the first argument to `dlsym' or `dlvsym' is set to RTLD_DEFAULT
   the run-time address of the symbol called NAME in the global scope
   is returned.  */
# define RTLD_DEFAULT	((void *) 0)


/* Type for namespace indeces.  */
typedef long int Lmid_t;

/* Special namespace ID values.  */
# define LM_ID_BASE	0	/* Initial namespace.  */
# define LM_ID_NEWLM	-1	/* For dlmopen: request new namespace.  */
#endif


void *dlopen(const char *filename, int flag);
char *dlerror(void);
void *dlsym(void *handle, const char *symbol);
int dlclose(void *handle);

#ifdef __USE_GNU
/* Like `dlopen', but request object to be allocated in a new namespace.  */
void *dlmopen (Lmid_t __nsid, const char *__file, int __mode);

/* Find the run-time address in the shared object HANDLE refers to
   of the symbol called NAME with VERSION.  */
void *dlvsym (void *__restrict __handle,
             const char *__restrict __name,
             const char *__restrict __version);
#endif


		//Link with -ldl.
