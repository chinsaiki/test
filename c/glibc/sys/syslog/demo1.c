/*
 * =====================================================================================
 *
 *       Filename:  demo1.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  21/03/13 21:20:18
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Zex (zex), top_zlynch@yahoo.com
 *   Organization:    /var/log/messages
 *
 * =====================================================================================
 */

#include <syslog.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
//  setlogmask( LOG_UPTO( LOG_INFO ) );
//	setlogmask( LOG_UPTO( LOG_WARNING ) );
//	setlogmask( LOG_MASK( LOG_WARNING ) );
//	setlogmask( LOG_MASK( LOG_DEBUG ) );

//	openlog( "/bin/ls", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_USER );
//	openlog( "say-my-name", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

	syslog( LOG_NOTICE, "with-syslog: Notice..." );
	syslog( LOG_ERR, "with-syslog: Error...%d", getuid() );
	syslog( LOG_WARNING, "with-syslog: Warning...%s", get_current_dir_name() );
//	syslog( LOG_DEBUG, "with-syslog: Debug...%f", gethostid() );
	syslog( LOG_DEBUG, "with-syslog: Debug...%f", 100.8 );

//	closelog();

	/* another connection */

//	openlog( "say-my-name", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

	syslog( LOG_NOTICE, "with-syslog: Notice..." );
	syslog( LOG_ERR, "with-syslog: Error...%d", getuid() );
	syslog( LOG_WARNING, "with-syslog: Warning...%s", get_current_dir_name() );

//	closelog();

	return 0;
}

