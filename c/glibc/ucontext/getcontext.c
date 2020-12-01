/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 06月 14日 星期五 09:05:34 CST. */
/* 
 Copyright (C) Rong Tao @Beijing

 Permission is granted to copy, distribute and/or modify this document
 under the terms of the GNU Free Documentation License, Version 1.3
 or any later version published by the Free Software Foundation;
 with no Invariant Sections, no Front-Cover Texts, and no Back-Cover
 Texts. A copy of the license is included in the section entitled ‘‘GNU
 Free Documentation License’’.
   2019年 03月 14日 星期四 19:24:35 CST. 
*/
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 13日 星期三 08:54:07 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 08日 星期五 08:10:19 CST. */
/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 07日 星期四 20:27:55 CST. */
/*

int getcontext (ucontext t *ucp) [Function]
The getcontext function initializes the variable pointed to by ucp with the context
of the calling thread. The context contains the content of the registers, the signal
mask, and the current stack. Executing the contents would start at the point where
the getcontext call just returned.
The function returns 0 if successful. Otherwise it returns -1 and sets errno accord
ingly.
The getcontext function is similar to setjmp but it does not provide an indication of
whether the function returns for the frst time or whether the initialized context was used
and the execution is resumed at just that point. If this is necessary the user has to take
determine this herself. This must be done carefully since the context contains registers
which might contain register variables. This is a good situation to defne variables with
volatile.
Once the context variable is initialized it can be used as is or it can be modifed. The
latter is normally done to implement co-routines or similar constructs. The makecontext
function is what has to be used to do that.
*/

#include <ucontext.h>
#include <stdio.h>

// struct ucontext_t{
    // ucontext_t *uc_link;
    // sigset_t uc_sigmask;
    // stack_t uc_stack;
    // mcontext_t uc_mcontext;
// };

int main()
{
    ucontext_t u;
    
    getcontext(&u);

    stack_t *stack = &u.uc_stack;

    printf("stack->ss_flags = %d\n", stack->ss_flags);
    printf("stack->ss_size = %d\n", stack->ss_size);
    
    return 0;
}