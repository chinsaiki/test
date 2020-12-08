#include <fcntl.h>

#include "config.h"


#define SYS_CONSOLE_LONG_BUF_LEN 4096
#define SYS_SCREEN_WIDTH 1024


INT VOS_Console_FD;
LONG cli_console_enable = 0;


long sys_console_buf_out( char * buf, int len )
{
    long ret =  fputs( buf,stdout );
    fflush(stdout );
    return ret;
}

long sys_console_printf( const char * format, ... )
{
  va_list args;
  int len;
  char buf[ SYS_SCREEN_WIDTH + 1 ];

  va_start( args, format );
  VOS_MemSet( buf, 0, ( SYS_SCREEN_WIDTH + 1 ) );
  len = VOS_Vsnprintf( buf, SYS_SCREEN_WIDTH,format, args );
  if ( ( len < 0 ) || ( len > SYS_SCREEN_WIDTH ) )
    {
      VOS_MemSet( buf, 0, ( SYS_SCREEN_WIDTH + 1 ) );
      len = VOS_Sprintf( buf, "Failed parse format string len(%d).\r\n", len );
    }

  va_end( args );

  sys_console_buf_out( buf, len );

  return len;

}


long sys_console_init_fd( void )
{
    CHAR cInit_Msg[20];
    
	VOS_MemZero( cInit_Msg, 20 );
    
	sys_console_printf("Linux console initialized successfully.\r\n");
	return VOS_Console_FD;
}

void sys_console_init ( void )
{
    sys_console_init_fd();
}

