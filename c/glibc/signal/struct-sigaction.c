/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 06月 14日 星期五 09:05:22 CST. */
/* 
 Copyright (C) Rong Tao @Beijing

 Permission is granted to copy, distribute and/or modify this document
 under the terms of the GNU Free Documentation License, Version 1.3
 or any later version published by the Free Software Foundation;
 with no Invariant Sections, no Front-Cover Texts, and no Back-Cover
 Texts. A copy of the license is included in the section entitled ‘‘GNU
 Free Documentation License’’.
   2019年 03月 14日 星期四 19:24:29 CST. 
*/
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 13日 星期三 08:54:00 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 08日 星期五 08:10:13 CST. */
/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 07日 星期四 20:27:51 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 07日 星期四 20:26:57 CST. */
/*
struct sigaction [Data Type]
Structures of type struct sigaction are used in the sigaction function to specify
all the information about how to handle a particular signal. This structure contains
at least the following members:
sighandler_t sa_handler
This is used in the same way as the action argument to the signal
function. The value can be SIG_DFL, SIG_IGN, or a function pointer. See
Section 24.3.1 [Basic Signal Handling], page 612.
sigset_t sa_mask
This specifes a set of signals to be blocked while the handler runs.
Blocking is explained in Section 24.7.5 [Blocking Signals for a Handler],
page 636. Note that the signal that was delivered is automatically blocked
by default before its handler is started; this is true regardless of the value
in sa_mask. If you want that signal not to be blocked within its handler,
you must write code in the handler to unblock it.
int sa_flags
This specifes various ﬂags which can aﬀect the behavior of the signal.
These are described in more detail in Section 24.3.5 [Flags for sigaction],
page 617.


*/

#include <signal.h>

struct sigaction{
    sighandler_t sa_handler;
    sigset_t sa_mask;
    int sa_flags;
};
struct sigaction{
	void     (*sa_handler)(int);
	void     (*sa_sigaction)(int, siginfo_t *, void *);
	sigset_t   sa_mask;
	int        sa_flags;
	void     (*sa_restorer)(void);
};
//Flags for sigaction
// int SA_NOCLDSTOP [Macro]
// This ﬂag is meaningful only for the SIGCHLD signal. When the ﬂag is set, the system
// delivers the signal for a terminated child process but not for one that is stopped. By
// default, SIGCHLD is delivered for both terminated children and stopped children.
// Setting this ﬂag for a signal other than SIGCHLD has no eﬀect.
// int SA_ONSTACK [Macro]
// If this ﬂag is set for a particular signal number, the system uses the signal stack
// when delivering that kind of signal. See Section 24.9 [Using a Separate Signal Stack],
// page 641. If a signal with this ﬂag arrives and you have not set a signal stack, the
// system terminates the program with SIGILL.
// int SA_RESTART [Macro]
// This ﬂag controls what happens when a signal is delivered during certain primitives
// (such as open, read or write), and the signal handler returns normally. There are
// two alternatives: the library function can resume, or it can return failure with error
// code EINTR.
// The choice is controlled by the SA_RESTART ﬂag for the particular kind of signal that
// was delivered. If the ﬂag is set, returning from a handler resumes the library function.
// If the ﬂag is clear, returning from a handler makes the function fail. See Section 24.5
// [Primitives Interrupted by Signals], page 628.







