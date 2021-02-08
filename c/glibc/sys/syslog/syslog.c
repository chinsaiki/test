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
 *	void syslog (int facility_priority, char *format, ...) [Function]
 *	syslog submits a message to the Syslog facility. It does this by writing to the Unix
 *	domain socket /dev/log.
 *	syslog submits the message with the facility and priority indicated by facil
 *	ity priority. The macro LOG_MAKEPRI generates a facility/priority from a facility and
 *	a priority, as in the following example:
 *	LOG_MAKEPRI(LOG_USER, LOG_WARNING)
 *	The possible values for the facility code are (macros):
 *	LOG_USER A miscellaneous user process
 *	LOG_MAIL Mail
 *	LOG_DAEMON
 *	A miscellaneous system daemon
 *	LOG_AUTH Security (authorization)
 *	LOG_SYSLOG
 *	Syslog
 *	LOG_LPR Central printer
 *	LOG_NEWS Network news (e.g. Usenet)
 *	LOG_UUCP UUCP
 *	LOG_CRON Cron and At
 *	LOG_AUTHPRIV
 *	Private security (authorization)
 *	LOG_FTP Ftp server
 *	LOG_LOCAL0
 *	Locally defned
 *	LOG_LOCAL1
 *	Locally defned
 *	LOG_LOCAL2
 *	Locally defned
 *	LOG_LOCAL3
 *	Locally defned
 *	LOG_LOCAL4
 *	Locally defned
 *	LOG_LOCAL5
 *	Locally defned
 *	LOG_LOCAL6
 *	Locally defned
 *	LOG_LOCAL7
 *	Locally defned
 *	Results are undefned if the facility code is anything else.
 *	NB: syslog recognizes one other facility code: that of the kernel. But you can’t
 *	specify that facility code with these functions. If you try, it looks the same to syslog
 *	as if you are requesting the default facility. But you wouldn’t want to anyway, because
 *	any program that uses the GNU C library is not the kernel.
 *	You can use just a priority code as facility priority. In that case, syslog assumes the
 *	default facility established when the Syslog connection was opened. See Section 18.2.5
 *	[Syslog Example], page 465.
 *	The possible values for the priority code are (macros):
 *	LOG_EMERG
 *	The message says the system is unusable.
 *	LOG_ALERT
 *	Action on the message must be taken immediately.
 *	LOG_CRIT The message states a critical condition.
 *	LOG_ERR The message describes an error.
 *	LOG_WARNING
 *	The message is a warning.
 *	LOG_NOTICE
 *	The message describes a normal but important event.
 *	LOG_INFO The message is purely informational.
 *	LOG_DEBUG
 *	The message is only for debugging purposes.
 *	Results are undefned if the priority code is anything else.
 *	If the process does not presently have a Syslog connection open (i.e., it did not call
 *	openlog), syslog implicitly opens the connection the same as openlog would, with
 *	the following defaults for information that would otherwise be included in an openlog
 *	call: The default identifcation string is the program name. The default default facility
 *	is LOG_USER. The default for all the connection options in options is as if those bits
 *	were oﬀ. syslog leaves the Syslog connection open.
 *	If the ‘dev/log’ socket is not open and connected, syslog opens and connects it, the
 *	same as openlog with the LOG_NDELAY option would.
 *	syslog leaves ‘/dev/log’ open and connected unless its attempt to send the message
*	failed, in which case syslog closes it (with the hope that a future implicit open will
		*	restore the Syslog connection to a usable state).
		 *	Example:
		 *	#include <syslog.h>
		 *	syslog (LOG_MAKEPRI(LOG_LOCAL1, LOG_ERROR),
				 *	"Unable to make network connection to %s. Error=%m", host);
*
*	
*
*
*/

#include <syslog.h>
void syslog(int priority, const char *message, ... );

#include <syslog.h>
#include <stdio.h>
#include <errno.h>



int main()
{
	//syslog (LOG_MAKEPRI(LOG_LOCAL1, LOG_ERROR),
	//		"syslog\n");

	setlogmask (LOG_UPTO (LOG_NOTICE));
	openlog ("exampleprog", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
	syslog (LOG_NOTICE, "Program started by User %d", getuid ());
	syslog (LOG_INFO, "A tree falls in a forest");
	closelog ();

	return 0;
}
