/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 06月 14日 星期五 09:05:14 CST. */
/* 
 Copyright (C) Rong Tao @Beijing

 Permission is granted to copy, distribute and/or modify this document
 under the terms of the GNU Free Documentation License, Version 1.3
 or any later version published by the Free Software Foundation;
 with no Invariant Sections, no Front-Cover Texts, and no Back-Cover
 Texts. A copy of the license is included in the section entitled ‘‘GNU
 Free Documentation License’’.
   2019年 03月 14日 星期四 19:24:25 CST. 
*/
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 13日 星期三 08:53:56 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 08日 星期五 08:10:10 CST. */
/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 07日 星期四 20:27:48 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 07日 星期四 20:26:55 CST. */
#include <stddef.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>


void with_other_locale (char *new_locale, void (*subroutine) (int), int argument)
{
    char *old_locale, *saved_locale;
    /* Get the name of the current locale. */
    old_locale = setlocale (LC_ALL, NULL);
    /* Copy the name so it won’t be clobbered by setlocale. */
    saved_locale = strdup (old_locale);
    if (saved_locale == NULL)
        printf ("Out of memory\n");
    /* Now change the locale and do some stuﬀ with it. */
    setlocale (LC_ALL, new_locale);
    (*subroutine) (argument);
    /* Restore the original locale. */
    setlocale (LC_ALL, saved_locale);
    
    free (saved_locale);
}

int main()
{
    return 0;
}