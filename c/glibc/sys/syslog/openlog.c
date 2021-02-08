/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 06月 14日 星期五 09:05:32 CST. */
/* 
 Copyright (C) Rong Tao @Beijing

 Permission is granted to copy, distribute and/or modify this document
 under the terms of the GNU Free Documentation License, Version 1.3
 or any later version published by the Free Software Foundation;
 with no Invariant Sections, no Front-Cover Texts, and no Back-Cover
 Texts. A copy of the license is included in the section entitled ‘‘GNU
 Free Documentation License’’.
   2019年 03月 14日 星期四 19:24:34 CST. 
*/
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 13日 星期三 08:54:05 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 08日 星期五 08:10:18 CST. */
/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 07日 星期四 20:27:55 CST. */
/*
18.2.1 openlog
The symbols referred to in this section are declared in the fle ‘syslog.h’.
void openlog (const char *ident, int option, int facility) [Function]
openlog opens or reopens a connection to Syslog in preparation for submitting mes
sages.
ident is an arbitrary identifcation string which future syslog invocations will prefx
to each message. This is intended to identify the source of the message, and people
conventionally set it to the name of the program that will submit the messages.
If ident is NULL, or if openlog is not called, the default identifcation string used in
Syslog messages will be the program name, taken from argv[0].
Please note that the string pointer ident will be retained internally by the Syslog
routines. You must not free the memory that ident points to. It is also dangerous to
pass a reference to an automatic variable since leaving the scope would mean ending
the lifetime of the variable. If you want to change the ident string, you must call
openlog again; overwriting the string pointed to by ident is not thread-safe.
You can cause the Syslog routines to drop the reference to ident and go back to
the default string (the program name taken from argv[0]), by calling closelog: See
Section 18.2.3 [closelog], page 464.
In particular, if you are writing code for a shared library that might get loaded and
then unloaded (e.g. a PAM module), and you use openlog, you must call closelog
before any point where your library might get unloaded, as in this example:
    #include <syslog.h>
    void
    shared_library_function (void)
    {
        openlog ("mylibrary", option, priority);
        syslog (LOG_INFO, "shared library has been invoked");
        closelog ();
    }
options is a bit string, with the bits as defned by the following single bit masks:
LOG_PERROR
    If on, openlog sets up the connection so that any syslog on this con
    nection writes its message to the calling process’ Standard Error stream
    in addition to submitting it to Syslog. If oﬀ, syslog does not write the
    message to Standard Error.
    
LOG_CONS If on, openlog sets up the connection so that a syslog on this connection
    that fails to submit a message to Syslog writes the message instead to
    system console. If oﬀ, syslog does not write to the system console (but
    of course Syslog may write messages it receives to the console).
    LOG_PID When on, openlog sets up the connection so that a syslog on this con
    nection inserts the calling process’ Process ID (PID) into the message.
    When oﬀ, openlog does not insert the PID.

LOG_NDELAY
    When on, openlog opens and connects the ‘/dev/log’ socket. When oﬀ,
    a future syslog call must open and connect the socket.
    Portability note: In early systems, the sense of this bit was exactly the
    opposite.
LOG_ODELAY
    This bit does nothing. It exists for backward compatibility.
    If any other bit in options is on, the result is undefned.
    
facility is the default facility code for this connection. A syslog on this connection
that specifes default facility causes this facility to be associated with the message.
See syslog for possible values. A value of zero means the default default, which is

LOG_USER.
    If a Syslog connection is already open when you call openlog, openlog “reopens” the
    connection. Reopening is like opening except that if you specify zero for the default
    facility code, the default facility code simply remains unchanged and if you specify
    LOG NDELAY and the socket is already open and connected, openlog just leaves it
    that way.
    
LOG_USER A miscellaneous user process
LOG_MAIL Mail
LOG_DAEMON  A miscellaneous system daemon
LOG_AUTH Security (authorization)
LOG_SYSLOG  Syslog
LOG_LPR Central printer
LOG_NEWS Network news (e.g. Usenet)
LOG_UUCP UUCP
LOG_CRON Cron and At
LOG_AUTHPRIV
Private security (authorization)
LOG_FTP Ftp server
LOG_LOCAL0
Locally defned
LOG_LOCAL1
Locally defned
LOG_LOCAL2
Locally defned
LOG_LOCAL3
Locally defned
LOG_LOCAL4
Locally defned
LOG_LOCAL5
Locally defned
LOG_LOCAL6
Locally defned
LOG_LOCAL7
Locally defned

18.2.4 setlogmask
The symbols referred to in this section are declared in the fle ‘syslog.h’.
int setlogmask (int mask) [Function]
setlogmask sets a mask (the “logmask”) that determines which future syslog calls
shall be ignored. If a program has not called setlogmask, syslog doesn’t ignore any
calls. You can use setlogmask to specify that messages of particular priorities shall
be ignored in the future.
A setlogmask call overrides any previous setlogmask call.
Note that the logmask exists entirely independently of opening and closing of Syslog
connections.
Setting the logmask has a similar eﬀect to, but is not the same as, confguring Syslog.
The Syslog confguration may cause Syslog to discard certain messages it receives,
but the logmask causes certain messages never to get submitted to Syslog in the frst
place.
mask is a bit string with one bit corresponding to each of the possible message pri
orities. If the bit is on, syslog handles messages of that priority normally. If it
is oﬀ, syslog discards messages of that priority. Use the message priority macros
described in Section 18.2.2 [syslog, vsyslog], page 462 and the LOG_MASK to construct
an appropriate mask value, as in this example:
LOG_MASK(LOG_EMERG) | LOG_MASK(LOG_ERROR)
or
~(LOG_MASK(LOG_INFO))
There is also a LOG_UPTO macro, which generates a mask with the bits on for a certain
priority and all priorities above it:
LOG_UPTO(LOG_ERROR)
The unfortunate naming of the macro is due to the fact that internally, higher numbers
are used for lower message priorities.
*/
#include <syslog.h>
void
shared_library_function (void)
{
    openlog ("mylibrary", LOG_PERROR, LOG_MAIL);
    syslog (LOG_INFO, "shared library has been invoked: %s, %m", __FILE__);
    syslog (LOG_ERR, "shared library has been invoked: %s", __FILE__);
    syslog (LOG_WARNING, "shared library has been invoked: %s", __FILE__);
    closelog ();
    
    openlog ("mylibrary2", LOG_PERROR, LOG_MAIL);
    syslog (LOG_INFO, "shared library has been invoked: %s", __FILE__);
    syslog (LOG_ERR, "shared library has been invoked: %s", __FILE__);
    syslog (LOG_WARNING, "shared library has been invoked: %s", __FILE__);
    closelog ();
    
    setlogmask (LOG_UPTO (LOG_NOTICE));
    openlog ("exampleprog", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
    syslog (LOG_NOTICE, "Program started by User %d", getuid ());
    syslog (LOG_INFO, "A tree falls in a forest");
    closelog ();
}

int main()
{
    shared_library_function();
    
    return 0;
}
