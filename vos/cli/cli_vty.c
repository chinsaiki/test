
#include <signal.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "config.h"
#include "buf.h"

#include "vos_vty.h"
#include "cl_vty.h"

#include "cli.h"
#include "cl_lib_sockunion.h"


LONG cli_telnet_port = 9898;

CHAR vos_hostname[VOS_SYSNAME_LEN]={"VHOST"};
CHAR vos_product_name[VOS_SYSNAME_LEN]={"VPRODUCT"};

extern INT VOS_Console_FD;
extern BOOL foreign_login_enable;

#define	VOS_CONSOLE_TASK_PRIORITY	__TASK_PRIORITY_NORMAL
/* telnet options */

#define TELOPT_BINARY    0    /* 8-bit data path */
#define TELOPT_ECHO    1    /* echo */
#define    TELOPT_RCP    2    /* prepare to reconnect */
#define    TELOPT_SGA    3    /* suppress go ahead */
#define    TELOPT_NAMS    4    /* approximate message size */
#define    TELOPT_STATUS    5    /* give status */
#define    TELOPT_TM    6    /* timing mark */
#define    TELOPT_RCTE    7    /* remote controlled transmission and echo */
#define TELOPT_NAOL     8    /* negotiate about output line width */
#define TELOPT_NAOP     9    /* negotiate about output page size */
#define TELOPT_NAOCRD    10    /* negotiate about CR disposition */
#define TELOPT_NAOHTS    11    /* negotiate about horizontal tabstops */
#define TELOPT_NAOHTD    12    /* negotiate about horizontal tab disposition */
#define TELOPT_NAOFFD    13    /* negotiate about formfeed disposition */
#define TELOPT_NAOVTS    14    /* negotiate about vertical tab stops */
#define TELOPT_NAOVTD    15    /* negotiate about vertical tab disposition */
#define TELOPT_NAOLFD    16    /* negotiate about output LF disposition */
#define TELOPT_XASCII    17    /* extended ascic character set */
#define    TELOPT_LOGOUT    18    /* force logout */
#define    TELOPT_BM    19    /* byte macro */
#define    TELOPT_DET    20    /* data entry terminal */
#define    TELOPT_SUPDUP    21    /* supdup protocol */
#define    TELOPT_SUPDUPOUTPUT 22    /* supdup output */
#define    TELOPT_SNDLOC    23    /* send location */
#define    TELOPT_TTYPE    24    /* terminal type */
#define    TELOPT_EOR    25    /* end or record */
#define TELOPT_EXOPL    255    /* extended-options-list */

#define VTY_PRE_ESCAPE 1
#define VTY_ESCAPE     2
#define sys_console_buf_out(a,b) VOS_Printf("%s",a);
char telnet_backward_char = 0x08;
char telnet_space_char = ' ';
extern ULONG g_ulRealToExecuteCommandFlag;
extern void linux_set_task_cancel_enable( void );
extern void linux_pthread_testcancel(void);
/*
#define _CL_RELAY_TELNET_OPTION_SERV_DEBUG_    
*/
#define VTY_TIMEOUT_EVENT_MAX 10
typedef int (*vty_timeout_callback_func)(struct vty *vty);  
vty_timeout_callback_func g_vty_timeout_callbacks[VTY_TIMEOUT_EVENT_MAX];

#ifdef _CL_RELAY_TELNET_OPTION_SERV_DEBUG_

struct vty *console_debug_vty;
#endif
#if 0
extern void cl_lib_thread_destroy_master ( struct cl_lib_thread_master * m );
#endif

/* cl_vector which store each vty structure. */
cl_vector cl_vtyvec_master = NULL;

/* VTY server thread. */
static cl_vector cl_vty_serv_thread;

/* Configure lock. */
static int cl_config_mode_lock;
int cl_config_session_count;
int cl_vty_session_count ;

int cl_vty_user_limit = 0 ;  /* 是否存在telnet用户个数的限制 1 －－ 有限制，0 －－ 没有限制 */

#if 0
delete by liuyanjun 2002 / 08 / 13
struct vty *cl_global_std_out_vty = NULL;
#endif

/* monitor vty_count */
static int cl_monitor_vty_count;

/* master of the threads. */
extern struct cl_lib_thread_master *cl_thread_master;


extern int cl_serv_telnet_fd;
extern int cl_serv_v6_telnet_fd;
extern int cl_serv_console_fd;

int vty_flush_window( struct vty * vty , int width, int height, int erase_flag, int no_more ) ;

typedef enum
{
    vty_timeout_code_min = 0,
    vty_timeout_code_profile,
    vty_timeout_code_max = 10
}g_vty_timeout_code_t;

vty_timeout_callback_func GetVtyTimeoutHandlerFunc(g_vty_timeout_code_t code);




#ifdef _DEBUG_VERSION_UNUSE_
extern void reset( void );
#endif

/**************************************
proto of functions
********************************/ 
/* Accept connection from the network. */
static int cl_vty_accept_telnet ( struct cl_lib_thread * thread );
static int cl_vty_serv_telnet( int sock );


static int vty_read ( struct cl_lib_thread * thread );

/* 采用vty->cl_vty_exit 来控制，避免并发访问
static int vty_exit (struct cl_lib_thread *thread);
*/

/* Flush vty buffer to the telnet or console. */
static int vty_flush ( struct cl_lib_thread * thread );

int vty_direct_out ( struct vty * vty , const char * format, ... );

/* When time out occur output message then close connection. */
int vty_timeout ( struct cl_lib_thread * thread );

static void vty_redraw_line ( struct vty * vty );

/* Kill line from the beginning. */
static void vty_kill_line_from_beginning ( struct vty * vty );

void cl_vty_serv_console_fd ( int console_fd );

extern int cl_serv_console_fd;

DECLARE_VOS_TASK( vty_sub_main )
{
    struct vty * m_vty = ( struct vty* ) ulArg1;
    struct cl_lib_thread thread;

    VOS_TaskInfoAttach();

    /* 在主任务中添加对命令行多任务的监视,包括串口的子任务和其他的telnet的子任务
       有子任务死掉或者没有响应,就杀掉这个子任务 */
    cliSetTaskFlagWhenTaskCreate( ( void * ) m_vty );

#if OS_LINUX
    linux_set_task_cancel_enable(); 
#endif

#if OS_LINUX
    linux_pthread_testcancel (); 
#endif

    while ( ( m_vty->sub_thread_master != NULL ) &&
            cl_lib_thread_fetch ( m_vty->sub_thread_master, &thread ) &&
            m_vty->cl_vty_exit == 0 )
    {
#if OS_LINUX
	    linux_pthread_testcancel (); 
#endif
        cl_lib_thread_call ( &thread );
#if OS_LINUX
	    linux_pthread_testcancel (); 
#endif
    }

    if ( ( m_vty != NULL ) && ( m_vty->cl_vty_exit == 1 ) ) /*m_vty可能已经在前面就被释放了*/
    {
        vty_task_exit( m_vty );
        return ;
    }

#if OS_LINUX
    linux_pthread_testcancel (); 
#endif

}

char *vty_get_user_name( struct vty * vty )
{
    return vty->user_name;
}

VOID vty_GetCurrentMode(struct vty *pVty, CHAR *pcMode)
{
    CHAR cView[]   = "VIEW";
    CHAR cConfig[] = "CONFIG";
    CHAR cTmp[]    = "TEMP_MODE";

    if((NULL == pVty) || (NULL == pcMode))
    {
        VOS_ASSERT(0);
        return;
    }
    VOS_MemZero(pcMode, 16); /*pcMode至少为20个字节*/
    if( VOS_CLI_VIEW_NODE == pVty->node )
    {
        VOS_StrCpy(pcMode, cView);
    }          
    else if( VOS_CLI_DEBUG_HIDDEN_NODE == pVty->node )
    {
        if(VOS_CLI_VIEW_NODE==pVty->prev_node) 
        {
			VOS_StrCpy(pcMode, cView);
        }
    	else
    	{
    		 VOS_StrCpy(pcMode, cConfig);
    	}
    } 
    else
    {
       VOS_StrCpy(pcMode, cTmp);
    }
    return;    
}

char *vty_get_user_ip( struct vty * vty )
{
    return vty->address;
}

//extern long Recv( int fd, void * ubuf, size_t size, unsigned flags );
#define Recv(a,b,c,d) read(a,b,c)
int cl_read( int fd, char * buf, int len )
{
    if ( fd <= 0 )
        return 0; 

    {
        if ( fd == VOS_Console_FD )     /* TODO:add aux console */
        {
#if (OS_LINUX)
		/* NO.012 ZHANGXINHUI FOR READ ERROR */
		return read( fd, buf, len );
		/*
		char tempChar;
		int i = 0;
		while(-1 != (tempChar = getc(fd))) 
		{
			buf[i++] = tempChar;
			if ( i == len )
				break;
		}
		return i;*/
 #endif
        }
        else    /* telnet */
        {
#ifdef OS_LINUX
            return recv( fd, buf, len, 0 );
#else
#endif

        }

    }
    return -1;
}
// extern long Send( int fd, void * buff, size_t len, unsigned flags );
#define Send(a,b,c,d) write(a,b,c)

int cl_write( int fd, char * buf, int len )
{
    if ( fd <= 0 )
        return 0; 

#if (OS_LINUX)
    write( fd, VOS_COLOR_NONE,VOS_StrLen(VOS_COLOR_NONE));

    {
        if ( fd == VOS_Console_FD )     /* TODO:add aux console */
        {
            return write( fd, buf, len );
        }
        else        /* telnet */
        {
            return write( fd, buf, len );
        }
    }
#endif
}

//extern long Writev( unsigned int fd, struct iovec * iov, int index );

int cl_writev( int fd, struct cl_lib_iovec * iovec, int index )
{
    if ( fd <= 0 )
        return 0; 

#if (OS_LINUX)

    {
        if ( fd == VOS_Console_FD )     /* TODO:add aux console */
        {
            return writev( fd, ( struct iovec * ) iovec, index );
        }
        else        /* telnet */
        {
            return writev( fd, ( struct iovec * ) iovec, index );
        }
    }
#endif
}
//extern long Close( unsigned int fd );

int cl_close( int fd )
{
    if ( fd <= 0 )
        return 0; 

#if (OS_LINUX)
    {
        return close( fd );
    }
#endif
}

extern int cl_relay_close( struct vty * vty );

static int vty_relay_exit( struct cl_lib_thread * thread )
{
    struct vty *vty = cl_lib_THREAD_ARG ( thread );

    vty_direct_out( vty, "\r\n" );
    vty_direct_prompt( vty );

	return(VOS_OK);
}

/*******************************************************
Basic functions of vty 
 
**********************************************/

/* Basic function to write buffer to vty. */
static void vty_write ( struct vty * vty, char * buf, unsigned long nbytes )
{
    /* Should we do buffering here ?  And make vty_flush (vty) ? */
    if ( VOS_SemTake( vty->Sem_obuf, _CL_VTY_OUT_SEM_WAIT_TIME_ ) == VOS_OK )
    {
        buffer_write ( vty->obuf, ( unsigned char * ) buf, nbytes );
        VOS_SemGive( vty->Sem_obuf );
    }
}

void vty_event ( enum event event, int sock, struct vty * vty )
{
    struct cl_lib_thread_master *thread_master;

    thread_master = ( vty == NULL ) ? cl_thread_master : vty->sub_thread_master;

    if ( thread_master == NULL )
    {
        VOS_ASSERT(0);
        return ;
    }

    vos_cli_lock();


    switch ( event )
    {
            /* 启动从fd中读数据的线程。 */
        case VTY_READ:
            {
                if (vty != NULL)
                {
                    vty->t_read = cl_lib_thread_add_read ( thread_master , vty_read, vty, sock );

                    /* Idle Time out treatment. */
                    if ( vty->v_timeout )
                    {
                        if ( vty->t_timeout )
                        {
                            cl_lib_thread_cancel ( vty->t_timeout );
                        }
#ifdef _CL_CMD_PTY_RELAY_
                        if ( vty->cl_vty_in_relay != 1 )
                        {
                            vty->t_timeout = NULL;
                        }
                        else
#endif
                        {
                            vty->t_timeout = cl_lib_thread_add_timer ( thread_master , vty_timeout, vty, vty->v_timeout );
                        }
                    }
                }
            }
            break;

        case VTY_READ_ALIVE:
            {
                if (vty != NULL )
                {
                    vty->t_read = cl_lib_thread_add_read ( thread_master , vty_read, vty, sock );
                }
            }
            break;

        case VTY_WRITE:
            {
                if (  vty != NULL )
                {
                    if ( !vty->t_write )
                    {
                        vty->t_write = cl_lib_thread_add_write ( thread_master , vty_flush, vty, sock );
                    }
                }
            }
            break;

        case VTY_TIMEOUT_RESET:
            {
                if ( vty != NULL )
                {
                    if ( vty->t_timeout )
                    {
                        cl_lib_thread_cancel ( vty->t_timeout );
                        vty->t_timeout = NULL;
                    }
                    #ifdef _CL_CMD_PTY_RELAY_
                    if ( ( vty->status != VTY_IOREDIRECT ) && ( vty->cl_vty_in_relay != 1 ) )
                    {
                        vty->t_timeout = NULL;
                    }
                    else
                    #endif
                    {
                        if ( vty->v_timeout )
                        {
                            vty->t_timeout =
                                cl_lib_thread_add_timer ( thread_master, vty_timeout, vty,
                                                          vty->v_timeout );
                        }
                    }
                }
            }
            break;
		case VTY_TELNET_RELAY_EXIT:
             if( vty != NULL )
             {
                 cl_lib_thread_add_event(thread_master, vty_relay_exit, vty, 0);
             }
			 break;
         default:
            break;
    }

    vos_cli_unlock();

}

/* Allocate new vty struct. */
struct vty *vty_new ( void )
{
    struct vty *new_vty = NULL;
    int i = 0;
    new_vty = ( struct vty * ) VOS_Malloc ( sizeof ( struct vty ), MODULE_RPU_CLI );
    if ( new_vty == NULL ) 
    {
        VOS_ASSERT( 0 );
        return NULL;
    }
    VOS_MemZero ( ( char * ) new_vty, sizeof ( struct vty ) );
    new_vty->obuf = ( struct buffer * ) buffer_new ( BUFFER_VTY, VTY_OUT_BUFSIZ );
    if ( new_vty->obuf == NULL )
    {
        VOS_ASSERT( 0 );
        VOS_Free( new_vty );
        return NULL;
    }
    new_vty->buf = ( char * ) VOS_Malloc ( VTY_BUFSIZ, MODULE_RPU_CLI );
    if ( new_vty->buf == NULL )
    {
        VOS_ASSERT( 0 );
        buffer_free( new_vty->obuf );
        VOS_Free( new_vty );
        return NULL;
    }
    new_vty->Sem_obuf = VOS_SemBCreate ( VOS_SEM_Q_FIFO , VOS_SEM_FULL );
    if ( new_vty->Sem_obuf == 0 )
    {
        VOS_ASSERT( 0 );
        VOS_Free( new_vty->buf );
        buffer_free( new_vty->obuf );
        VOS_Free( new_vty );
        return NULL;
    }
    new_vty->max = VTY_BUFSIZ;
    new_vty->sb_buffer = NULL;
#ifdef _CL_CMD_PTY_RELAY_    
    new_vty->t_relay_read = NULL;
    new_vty->cl_relay_sb_buffer = NULL;
    new_vty->t_relay_keep_alive_timeout = NULL;
    new_vty->cl_relay_thread_master = NULL;
    new_vty->cl_telnet_taskid = NULL;
    new_vty->sub_task_id = NULL;
#endif
    new_vty->debug_monitor = 0;
    new_vty->frozen = 0;

    new_vty->sub_thread_master = NULL;
    new_vty->ncount = 1;

    for( i = 0; i < 8; i++)
  	{
  	   new_vty->node_his_save[i].node_in_use = 0;
  	}

    #if 0 // syslog
    VOS_MemZero( ( char * ) &( new_vty->monitor_conf ), sizeof( VOS_SYSLOG_MONITOR_CONF ) );
    new_vty->monitor_conf.cMonitor_Level = 7;

    new_vty->monitor_conf.cMonitor_Type_State[ LOG_TYPE_TRAP ] = 1;  /*对于TRAP 类型默认是打开的*/
    #endif
    return new_vty;
}

/* 对于vty_new()，要使用vty_free();
   对于vty_create(), 要使用vty_close() */
void vty_free( struct vty * vty )
{
    if ( vty->obuf )
        buffer_free ( vty->obuf );

    if ( vty->buf )
        VOS_Free( vty->buf );

    if ( vty->Sem_obuf )
    {
        VOS_SemDelete( vty->Sem_obuf ) ;
    }


    VOS_Free( vty );
    vty = NULL;
}



/* Ensure length of input buffer.  Is buffer is short, double it. */
/*static void vty_ensure (struct vty *vty, int length)*/
static int vty_ensure ( struct vty * vty, int length )
{
    if ( vty->max <= length )
    {
        char * pr = NULL ;
        pr = ( char * ) cl_realloc( vty->buf, vty->max, ( vty->max ) * 2, MODULE_RPU_CLI );
        if ( pr == NULL )
            return -1;

        vty->buf = pr ;
        vty->max *= 2;
        /*
        vty->buf = XREALLOC (MTYPE_VTY, vty->buf, vty->max);
        */
    }
    return 1;
}

/*******************************************
  vty input and output related functions
 
*****************************************/
void vty_clear_buf ( struct vty * vty )
{
    VOS_MemZero ( ( char * ) vty->buf, vty->max );
}

/* Quit print out to the buffer. */
void vty_buffer_reset_out ( struct vty * vty )
{
    vty_buffer_reset( vty );
    vty_prompt ( vty );
    vty_redraw_line ( vty );
}
char* vty_to_buffer(struct vty *vty) 
{
    char *myBuf = NULL;
    int len = 0;

    if((vty == NULL) || (vty->obuf == NULL) || (vty->obuf->length == 0))
      return NULL;

    myBuf = VOS_Malloc(vty->obuf->length+32, MODULE_RPU_CLI);
    if(myBuf != NULL)
    {
        struct buffer_data *start = NULL;
        struct buffer_data *data = NULL;

        VOS_MemSet(myBuf, 0, vty->obuf->length+32);
        
        /*写入接口配置*/
        start = vty->obuf->head;
    	for ( data = start;data;data = data->next )
    	{
    		VOS_MemCpy(&myBuf[len], data->data, data->cp );
            len += data->cp;
    	}
    }
    VOS_ASSERT(len <= (vty->obuf->length+32));
    return myBuf;
}
/******************************************
vty combination output little functions 
*********************************************/

/* Say hello to vty interface. */
void vty_hello ( struct vty * vty )
{
    VOS_LOCAL_TIME vos_time; 
    char addr[128] = {0};
    {
        if(cl_serv_console_fd == vty->fd)
        {
            VOS_Sprintf(addr,"%s",_CL_CONSOLE_NAME_);
        }
        else
        {
            union cl_lib_sockunion *su = cl_lib_sockunion_getpeername (vty->fd);
            if(su)
            {
                cl_lib_inet_sutop(su,addr);
                VOS_Free(su);
            }
        }
    }
    
    VOS_MemSet(&vos_time,0,sizeof(vos_time));
    VOS_local_time(&vos_time);
//    vty_out(vty,"****** welcome,");
    vty_out(vty,VOS_COLOR_BLUE"client %s login at %04d-%02d-%02d %02d:%02d:%02d\r\n",
        addr,
        vos_time.usYear,vos_time.usMonth,vos_time.usDay,
        vos_time.usHour,vos_time.usMinute,vos_time.usSecond);
    vty_out( vty,VOS_COLOR_NONE" ");

    vty_out( vty, "%s\r\n",VOS_get_cli_welcomeStr());

    return ;
}

/* add for the INTERFACE_NODE,VLAN_NODE,SUPER_VLAN_NODE,TRUNK_NODE,TUNNEL_NODE,LSP_NODE,
LOOPBACK_NODE,POS_NODE prompt when two or more user in the switch by liuyanjun 2002/10/14 */
VOID vty_set_prompt( struct vty * vty, CHAR cpPrompt[VTY_CMD_PROMPT_MAX] )
{
    if ( cpPrompt == NULL )
    {
        VOS_ASSERT( 0 );
        return ;
    }
    VOS_MemZero( vty->vty_cmd_prompt, 64 );
    if ( VOS_StrLen( cpPrompt ) >= 64 - 1 )
    {
        VOS_ASSERT( 0 );
        return ;
    }
    VOS_StrnCpy( vty->vty_cmd_prompt, cpPrompt, 64 - 1 );

    return ;
}

/* Put out prompt and wait input from user. */
void vty_prompt ( struct vty * vty )
{
    char *hostname;
    if ( ( vty->type == VTY_TERM )
            && ( vty->status != VTY_CLOSE ) )
    {
        hostname = VOS_get_hostname();
        /* 自动打印hostname */
        #if 1
        vty_out ( vty,"%s", hostname );
        if(VOS_StrLen(vty->vty_cmd_prompt) > 2)
        {
            vty_out ( vty,"%s",vty->vty_cmd_prompt);
        }
        else
        {
            vty_out ( vty,"%s",cmd_prompt ( vty->node ));
        }
        #else
        /* 至少需要包含 %s 用于 hostname */
        if(VOS_StrLen(vty->vty_cmd_prompt) > 2)
        {
            vty_out ( vty, vty->vty_cmd_prompt, hostname );
        }
        else
        {
            vty_out ( vty, cmd_prompt ( vty->node ), hostname );
        }
        #endif
    }
    return ;
}

void vty_direct_prompt( struct vty * vty )
{
    char *hostname;

    if ( ( vty->type == VTY_TERM )
            && ( vty->status != VTY_CLOSE ) )
    {
        hostname = VOS_get_hostname();
        vty_direct_out ( vty, cmd_prompt ( vty->node ), hostname );
        /*vty_direct_out ( vty, vty->vty_cmd_prompt, hostname );*/
    }
    return ;
}

void cl_vty_clear_screen( struct vty *vty )
{
    char clear_str[ 20 ] = "\x1b\x5b\x30\x6d\x1b\x5b\x30\x3b\x30\x66\x1b\x5b\x32J";

    if(_CL_VTY_SSH_ == vty->conn_type)
    {
        vty_out(vty, clear_str);
    }
    else
    {
        cl_write( vty->fd, clear_str, VOS_StrLen( clear_str ) );
    }

    return ;
}

/******************************************************************
Function: push_vty_his_node
Description: 
          save the  current node as history node in the struct node_his_save[8]
          return VOS_OK     success
          return VOS_ERROR  failure
*******************************************************************/
LONG  vty_enter_node(struct vty *vty,enum node_type newnode)
{
    int i = 0;
    for( i = 0; i < VTY_NODE_HIS_MAX; i++)
    {
        if(vty->node_his_save[i].node_in_use == 0)
        {
            vty->node_his_save[i].node_in_use = 1;
            vty->node_his_save[i].node = vty->node;
            vty->node_his_save[i].context_data = vty->context_data;
            vty->node_his_save[i].context_data_need_free = vty->context_data_need_free;
            VOS_StrCpy(vty->node_his_save[i].vty_cmd_prompt,vty->vty_cmd_prompt);
            
            vty->node = newnode;
            return VOS_OK;
        }
    } 
  return VOS_ERROR;
 } 

/******************************************************************
Function: pop_vty_his_node
Description: 
          get  the last node as exit node from the struct node_his_save[8]
          return VOS_OK     success
          return VOS_ERROR  failure
*******************************************************************/
LONG  vty_exit_node(struct vty *vty)
{
    int i=0;
    for( i = VTY_NODE_HIS_MAX - 1; i >= 0; i--)
    {
        if(vty->node_his_save[i].node_in_use == 1)
        {
            vty->node_his_save[i].node_in_use = 0;
            vty->node                    = vty->node_his_save[i].node;
            vty->context_data            = vty->node_his_save[i].context_data;
            vty->context_data_need_free  = vty->node_his_save[i].context_data_need_free;
            VOS_StrCpy(vty->vty_cmd_prompt,vty->node_his_save[i].vty_cmd_prompt);

            VOS_MemZero(&vty->node_his_save[i],sizeof(vty->node_his_save[i]));
            return VOS_OK;
        }
    } 
    return VOS_ERROR;
} 

void cl_vty_up_line( int fd )
{
    char up_line_str[ 20 ] = "\x1b\x5b\x33\x3b\x31\x48";

    cl_write( fd, up_line_str, VOS_StrLen( up_line_str ) );

    return ;
}


void cl_vty_page_head( int fd )
{
    char page_head_str[ 20 ] = "\x1b\x5b\x31\x3b\x31\x48";

    cl_write( fd, page_head_str, VOS_StrLen( page_head_str ) );

    return ;
}

void cl_vty_color_out( struct vty * vty, char * str )
{
    char color_pre[] = "\x1b\x5b\x37\x6d";
    char color_end[] = "\x1b\x5b\x6d";

    vty_out( vty, "%s", color_pre );
    vty_out( vty, "%s", str );
    vty_out( vty, "%s", color_end );

    return ;
}

/*****************************************************
 Telnet options related functions
 
******************************************************/ 
/* This function redraw all of the command line character. */
static void vty_redraw_line ( struct vty * vty )
{
    vty_write ( vty, vty->buf, vty->length );
    vty->cp = vty->length;
}

/* Send WILL TELOPT_ECHO to remote server. */
void vty_will_echo ( struct vty * vty )
{
    unsigned char cmd[] = { ( unsigned char ) IAC, ( unsigned char ) WILL, ( unsigned char ) TELOPT_ECHO, '\0' };
    vty_out ( vty, "%s", cmd );
}

void vty_epon( struct vty * vty )
{
    unsigned char cmd[] = { ( unsigned char ) IAC, ( unsigned char ) DO, ( unsigned char ) TELOPT_EPON, '\0' };
    vty_direct_out ( vty, "%s", cmd );
}

void vty_ayt( struct vty * vty )
{
    unsigned char cmd1[] = { ( unsigned char ) IAC, ( unsigned char ) AYT, '\0' };
    vty_out ( vty, "%s", cmd1 );
}

void vty_ack_ayt( struct vty * vty )
{
    /*
    char cmd[] = { (char) IAC, (char) YES, '\0'};
    vty_out(vty,"%s",cmd);
    */
}

#if 1 
/* Make suppress Go-Ahead telnet option. */
static void vty_will_suppress_go_ahead ( struct vty * vty )
{
    unsigned char cmd1[] = { ( unsigned char ) IAC, ( unsigned char ) WILL, ( unsigned char ) TELOPT_SGA, '\0' };
    vty_out ( vty, "%s", cmd1 );

}
#endif

#if 1 
/* Make don't use linemode over telnet. */
static void vty_dont_linemode ( struct vty * vty )
{
    unsigned char cmd[] = { ( unsigned char ) IAC, ( unsigned char ) DONT, ( unsigned char ) TELOPT_LINEMODE, '\0' };
    vty_out ( vty, "%s", cmd );
}
#endif

#if 1 
/* Use window size. */
static void vty_do_window_size ( struct vty * vty )
{
    unsigned char cmd[] = { ( unsigned char ) IAC, ( unsigned char ) DO, ( unsigned char ) TELOPT_NAWS, '\0' };
    vty_out ( vty, "%s", cmd );
}
#endif

/* Basic function to insert character into vty. */
static void vty_self_insert ( struct vty * vty, char c )
{
    int i;
    int length;

    if ( vty->length >= _CL_MAX_COMMAND_LENGTH_ )
    {
        vty_out( vty, "%% Error, Too long input command.%s", VTY_NEWLINE );
        /* Clear command line buffer. */
        vty->cp = vty->length = 0;
        vty_clear_buf ( vty );
        if ( vty->status != VTY_CLOSE
                && vty->status != VTY_START && vty->status != VTY_CONTINUE )
        {
            vty_prompt ( vty );
        }

        return ;
    }

    /*vty_ensure (vty, vty->length + 1);*/
    if ( vty_ensure ( vty, vty->length + 1 ) < 0 )
        return ;
    length = vty->length - vty->cp;
    VOS_MemMove ( &vty->buf[ vty->cp + 1 ], &vty->buf[ vty->cp ], length );
    vty->buf[ vty->cp ] = c;
    vty_write ( vty, &vty->buf[ vty->cp ], length + 1 );
    for ( i = 0; i < length; i++ )
    {
        vty_write ( vty, &telnet_backward_char, 1 );
    }
    vty->cp++;
    vty->length++;
}

/* Self insert character 'c' in overwrite mode. */
static void vty_self_insert_overwrite ( struct vty * vty, char c )
{
    /*vty_ensure (vty, vty->length + 1);*/ 
    if ( vty_ensure ( vty, vty->length + 1 ) < 0 )
        return ;
    vty->buf[ vty->cp++ ] = c;
    if ( vty->cp > vty->length )
    {
        vty->length++;
    }
    vty_write ( vty, &c, 1 );
}

/* Insert a word into vty interface with overwrite mode. */
static void vty_insert_word_overwrite ( struct vty * vty, char * str )
{
    int len = VOS_StrLen ( str );
    vty_write ( vty, str, len );
    VOS_StrCpy( &vty->buf[ vty->cp ], str );
    vty->cp += len;
    vty->length = vty->cp;
}

/* Forward character. */
static void vty_forward_char ( struct vty * vty )
{
    if ( vty->cp < vty->length )
    {
        vty_write ( vty, &vty->buf[ vty->cp ], 1 );
        vty->cp++;
    }
}

/* Backward character. */
static void vty_backward_char ( struct vty * vty )
{
    if ( vty->cp > 0 )
    {
        vty->cp--;
        vty_write ( vty, &telnet_backward_char, 1 );
    }
}


/* Move to the beginning of the line. */
static void vty_beginning_of_line ( struct vty * vty )
{
    while ( vty->cp )
    {
        vty_backward_char ( vty );
    }
}

/* Move to the end of the line. */
static void vty_end_of_line ( struct vty * vty )
{
    while ( vty->cp < vty->length )
    {
        vty_forward_char ( vty );
    }
}

/* Print command line history.  This function is called from
   vty_next_line and vty_previous_line. */
static void vty_history_print ( struct vty * vty )
{
    int length;

    vty_kill_line_from_beginning ( vty );

    /* Get previous line from history buffer */
    length = VOS_StrLen ( vty->hist[ vty->hp ] );
    VOS_MemCpy ( vty->buf, vty->hist[ vty->hp ], length );
    vty->cp = vty->length = length;

    /* Redraw current line */
    vty_redraw_line ( vty );
}


/* Show next command line history. */
void vty_next_line ( struct vty * vty )
{
    int try_index;

    if ( vty->hp == vty->hindex )
    {
        return ;
    }

    /* Try is there history exist or not. */
    try_index = vty->hp;
    if ( try_index == ( VTY_MAXHIST - 1 ) )
    {
        try_index = 0;
    }
    else
    {
        try_index++;
    }

    /* If there is not history return. */
    if ( vty->hist[ try_index ] == NULL )
    {
        return ;
    }
    else
    {
        vty->hp = try_index;
    }

    vty_history_print ( vty );
}


/* Show previous command line history. */
void vty_previous_line ( struct vty * vty )
{
    int try_index;

    try_index = vty->hp;
    if ( try_index == 0 )
    {
        try_index = VTY_MAXHIST - 1;
    }
    else
    {
        try_index--;
    }
    if ( vty->hist[ try_index ] == NULL )
    {
        return ;
    }
    else
    {
        vty->hp = try_index;
    }
    vty_history_print ( vty );
}

/* Forward word. */
static void vty_forward_word ( struct vty * vty )
{
    while ( vty->cp != vty->length && vty->buf[ vty->cp ] != ' ' )
    {
        vty_forward_char ( vty );
    }
    while ( vty->cp != vty->length && vty->buf[ vty->cp ] == ' ' )
    {
        vty_forward_char ( vty );
    }
}


/* Backward word without skipping training space. */
static void vty_backward_pure_word ( struct vty * vty )
{
    while ( vty->cp > 0 && vty->buf[ vty->cp - 1 ] != ' ' )
    {
        vty_backward_char ( vty );
    }
}


/* Backward word. */
static void vty_backward_word ( struct vty * vty )
{
    while ( vty->cp > 0 && vty->buf[ vty->cp - 1 ] == ' ' )
    {
        vty_backward_char ( vty );
    }
    while ( vty->cp > 0 && vty->buf[ vty->cp - 1 ] != ' ' )
    {
        vty_backward_char ( vty );
    }
}

#if 0 
/* When '^D' is typed at the beginning of the line we move to the down
   level. */
static void
vty_down_level ( struct vty * vty )
{
    vty_out ( vty, "%s", VTY_NEWLINE );
    config_exit ( NULL, vty, 0, NULL );
    vty_prompt ( vty );
    vty->cp = 0;
}
#endif

/* When '^Z' is received from vty, move down to the enable mode. */
void vty_end_config ( struct vty * vty )
{
    vty_out ( vty, "%s", VTY_NEWLINE );
    switch ( vty->node )
    {
        case VOS_CLI_VIEW_NODE:
            {
                /*
                * Nothing to do. *
                */
            }
            break;

        case VOS_CLI_CONFIG_NODE:
            {
                vty_config_unlock ( vty );
                vty->node = VOS_CLI_VIEW_NODE;
            }
            break;

        default:
            /* Unknown node, we have to ignore it. */
            break;
    }

    vty_clear_buf ( vty );
    vty->cp = vty->length = 0;
    vty_prompt ( vty );
}

/* Delete a charcter at the current point. */
static void vty_delete_char ( struct vty * vty )
{
    int i;
    int size;

    if ( vty->cp == vty->length )
    {
        return ;            /* completion need here? */
    }
    size = vty->length - vty->cp;
    vty->length--;
    VOS_MemMove ( &vty->buf[ vty->cp ], &vty->buf[ vty->cp + 1 ], size - 1 );
    vty->buf[ vty->length ] = '\0';
    vty_write ( vty, &vty->buf[ vty->cp ], size - 1 );
    vty_write ( vty, &telnet_space_char, 1 );
    for ( i = 0; i < size; i++ )
    {
        vty_write ( vty, &telnet_backward_char, 1 );
    }
}

/* Delete a character before the point. */
static void vty_delete_backward_char ( struct vty * vty )
{
    if ( vty->cp == 0 )
    {
        return ;
    }
    vty_backward_char ( vty );
    vty_delete_char ( vty );
}

/* Kill rest of line from current point. */
static void vty_kill_line ( struct vty * vty )
{
    int i;
    int size;

    size = vty->length - vty->cp;
    if ( size == 0 )
    {
        return ;
    }
    for ( i = 0; i < size; i++ )
    {
        vty_write ( vty, &telnet_space_char, 1 );
    }
    for ( i = 0; i < size; i++ )
    {
        vty_write ( vty, &telnet_backward_char, 1 );
    }
    VOS_MemZero ( ( char * ) &vty->buf[ vty->cp ], size );
    vty->length = vty->cp;
}

/* Kill line from the beginning. */
static void vty_kill_line_from_beginning ( struct vty * vty )
{
    vty_beginning_of_line ( vty );
    vty_kill_line ( vty );
}

/* Delete a word before the point. */
static void vty_forward_kill_word ( struct vty * vty )
{
    while ( vty->cp != vty->length && vty->buf[ vty->cp ] == ' ' )
    {
        vty_delete_char ( vty );
    }
    while ( vty->cp != vty->length && vty->buf[ vty->cp ] != ' ' )
    {
        vty_delete_char ( vty );
    }
}

/* Delete a word before the point. */
static void vty_backward_kill_word ( struct vty * vty )
{
    while ( vty->cp > 0 && vty->buf[ vty->cp - 1 ] == ' ' )
    {
        vty_delete_backward_char ( vty );
    }
    while ( vty->cp > 0 && vty->buf[ vty->cp - 1 ] != ' ' )
    {
        vty_delete_backward_char ( vty );
    }
}

/* Transpose chars before or at the point. */
static void vty_transpose_chars ( struct vty * vty )
{
    char c1, c2;

    /* If length is short or point is near by the beginning of line then
    return. */
    if ( vty->length < 2 || vty->cp < 1 )
    {
        return ;
    }

    /* In case of point is located at the end of the line. */
    if ( vty->cp == vty->length )
    {
        c1 = vty->buf[ vty->cp - 1 ];
        c2 = vty->buf[ vty->cp - 2 ];
        vty_backward_char ( vty );
        vty_backward_char ( vty );
        vty_self_insert_overwrite ( vty, c1 );
        vty_self_insert_overwrite ( vty, c2 );
    }
    else
    {
        c1 = vty->buf[ vty->cp ];
        c2 = vty->buf[ vty->cp - 1 ];
        vty_backward_char ( vty );
        vty_self_insert_overwrite ( vty, c1 );
        vty_self_insert_overwrite ( vty, c2 );
    }
}

/* ^C stop current input and do not add command line to the history. */
static void vty_stop_input ( struct vty * vty )
{
    vty->cp = vty->length = 0;
    vty_clear_buf ( vty );
    vty_out ( vty, "%s", VTY_NEWLINE );
    switch ( vty->node )
    {
        case VOS_CLI_VIEW_NODE:
            {
                /*    case ENABLE_NODE:

                * Nothing to do. */
            }
            break;

        case VOS_CLI_CONFIG_NODE:
            {
                vty_config_unlock ( vty );
                vty->node = VOS_CLI_VIEW_NODE;
            }
            break;

        case VOS_CLI_DEBUG_HIDDEN_NODE:
            {
                vty->node = VOS_CLI_VIEW_NODE;
            }
            break;

        default:
            /* Unknown node, we have to ignore it. */
            break;
    }

    vty_prompt ( vty );

    /* Set history pointer to the latest one. */
    vty->hp = vty->hindex;
}

/* Add current command line to the history buffer. */
static void vty_hist_add ( struct vty * vty )
{
    int index;

    if ( vty->length == 0 )
    {
        return ;
    }
    index = vty->hindex ? vty->hindex - 1 : VTY_MAXHIST - 1;

    /* Ignore the same string as previous one. */
    if ( vty->hist[ index ] )
    {
        if ( VOS_StrCmp ( vty->buf, vty->hist[ index ] ) == 0 )
        {
            vty->hp = vty->hindex;
            return ;
        }
    }

    /* Insert history entry. */
    if ( vty->hist[ vty->hindex ] )
    {
        VOS_Free ( vty->hist[ vty->hindex ] );
    }
    vty->hist[ vty->hindex ] = VOS_StrDup ( vty->buf, MODULE_RPU_CLI );
    if ( vty->hist[ vty->hindex ] == NULL )
    {
        VOS_ASSERT( 0 );
        return ;
    }

    /* History index rotation. */
    vty->hindex++;
    if ( vty->hindex == VTY_MAXHIST )
    {
        vty->hindex = 0;
    }
    vty->hp = vty->hindex;
}

/* #define _CL_TELNET_OPTION_DEBUG_ */
/* Get telnet window size. */
static int vty_telnet_option ( struct vty * vty, unsigned char * buf, int nbytes )
{
    switch ( buf[ 0 ] )
    {
        case WONT:
            if ( buf[ 1 ] == TELOPT_EPON )
            {}
            break;

        case SB:
            {
                buffer_reset ( vty->sb_buffer );
                vty->iac_sb_in_progress = 1;
                return 0;
            }

        case SE:
            {
                char *buffer = ( char * ) vty->sb_buffer->head->data;
                int length = vty->sb_buffer->length;
                if ( buffer == NULL )
                {
                    return 0;
                }
                if ( !vty->iac_sb_in_progress )
                {
                    return 0;
                }
                if ( buffer[ 0 ] == '\0' )
                {
                    vty->iac_sb_in_progress = 0;
                    return 0;
                }
                switch ( buffer[ 0 ] )
                {
                    case TELOPT_NAWS:
                        {
                            if ( length < 5 )
                            {
                                break;
                            }

                            vty->width = buffer[ 2 ];
                            vty->height = vty->lines >= 0 ? vty->lines : buffer[ 4 ];
                        }
                        break;
                }
                vty->iac_sb_in_progress = 0;
                return 0;
            }

        case AYT:
            {
                vty_ack_ayt( vty );
            }
            break;

        default:
            break;
    }

    return 1;
}

/* Escape character command map. */
static void vty_escape_map ( unsigned char c, struct vty * vty )
{
    switch ( c )
    {
        case ( 'A' ) :
            {
                vty_previous_line ( vty );
            }
            break;

        case ( 'B' ) :
            {
                vty_next_line ( vty );
            }
            break;

        case ( 'C' ) :
            {
                vty_forward_char ( vty );
            }
            break;

        case ( 'D' ) :
            {
                vty_backward_char ( vty );
            }
            break;

        default:
            break;
    }

    /* Go back to normal mode. */
    vty->escape = VTY_NORMAL;
}

/***********************************************
vty user manage related  function 
 
**********************************************/

/*****************************************************
 
vty call command parser to execute and describe command
 
vty_command:
**********************************************************/ 
/* Do completion at vty interface. */

/** 
 * 命令补全
 * @param[in]   vty             当前会话
 */
static void vty_complete_command ( struct vty * vty )
{
    int i;
    LONG ret;
    char **matched = NULL;
    cl_vector vline;
    char *lowbuf;

    lowbuf = VOS_StrDupToLower( vty->buf, MODULE_RPU_CLI );
    if ( lowbuf == NULL )
    {
        return ;
    }
    vline = cmd_make_strvec ( lowbuf );
    if ( vline == NULL )
    {
        VOS_Free( lowbuf );
        return ;
    }

    /* In case of 'help \t'. */
    if ( vos_isspace ( ( int ) vty->buf[ vty->length - 1 ] ) )
    {
        cl_vector_set ( vline, NULL );
    }
    vty_out ( vty, "%s", VTY_NEWLINE );
    matched = cmd_complete_command ( vline, vty, &ret );
    cmd_free_strvec ( vline );
    switch ( ret )
    {
        case CMD_ERR_AMBIGUOUS:
            {
                vty_out ( vty, "%% Error, Ambiguous command.%s", VTY_NEWLINE );
                vty_prompt ( vty );
                vty_redraw_line ( vty );
            }
            break;

        case CMD_ERR_NO_MATCH:
            {
                vty_out ( vty, "%% Error, There is no matched command.%s", VTY_NEWLINE );
                /*标题:unknown command等提示没有缩进2个字符*/
                vty_prompt ( vty );
                vty_redraw_line ( vty );
            }
            break;

        case CMD_COMPLETE_FULL_MATCH:
            {
                vty_prompt ( vty );
                vty_redraw_line ( vty );
                vty_backward_pure_word ( vty );
                vty_insert_word_overwrite ( vty, matched[ 0 ] );
                vty_self_insert ( vty, ' ' );
                VOS_Free ( matched[ 0 ] );
            }
            break;

        case CMD_COMPLETE_MATCH:
            {
                vty_prompt ( vty );
                vty_redraw_line ( vty );
                vty_backward_pure_word ( vty );
                vty_insert_word_overwrite ( vty, matched[ 0 ] );
                VOS_Free ( matched[ 0 ] );
            }
            break;

        case CMD_COMPLETE_LIST_MATCH:
            {
                for ( i = 0; matched[ i ] != NULL; i++ )
                {
                    /* 把tab键的命令片断补齐改为每行4个,每个18个字节长,liuyanjun 2002/10/11 */
                    if ( i != 0 && ( ( i % 4 ) == 0 ) )
                    {
                        vty_out ( vty, "%s", VTY_NEWLINE );
                    }
                    vty_out ( vty, "%-18s ", matched[ i ] );
                    VOS_Free ( matched[ i ] );
                }
                vty_out ( vty, "%s", VTY_NEWLINE );
                vty_prompt ( vty );
                vty_redraw_line ( vty );
            }
            break;

        case CMD_ERR_NOTHING_TODO:
            {
                vty_prompt ( vty );
                vty_redraw_line ( vty );
            }
            break;

        default:
            break;
    }

    if ( matched )
    {
        cl_vector_only_index_free ( matched );
    }
    VOS_Free( lowbuf );

    return ;
}

void vty_describe_fold ( struct vty * vty, int cmd_width, unsigned int desc_width,
                         struct desc * desc )
{
    char *buf, *cmd, *p;
    unsigned int pos;

    cmd = desc->cmd[ 0 ] == '.' ? desc->cmd + 1 : desc->cmd;
    if ( desc_width <= 0 )
    {
        vty_out ( vty, "  %-*s  %s%s", cmd_width, cmd, desc->str, VTY_NEWLINE );
        return ;
    }
    buf = ( char * ) VOS_Malloc( ( VOS_StrLen ( desc->str ) + 1 ), MODULE_RPU_CLI );
    if ( buf == NULL )
    {
        VOS_ASSERT( 0 );
        return ;
    }
    for ( p = desc->str; VOS_StrLen ( p ) > desc_width; p += pos + 1 )
    {
        for ( pos = desc_width; pos > 0; pos-- )
        {
            if ( *( p + pos ) == ' ' )
            {
                break;
            }
        }
        if ( pos == 0 )
        {
            break;
        }
        VOS_StrnCpy ( buf, p, pos );
        buf[ pos ] = '\0';
        vty_out ( vty, "  %-*s  %s%s", cmd_width, cmd, buf, VTY_NEWLINE );
        cmd = "";
    }
    vty_out ( vty, "  %-*s  %s%s", cmd_width, cmd, p, VTY_NEWLINE );
    VOS_Free ( buf );
}

/************************************
检查缓存中是否含有注释符号或其他非法字符
0  OK
1  字符串的第一个字母为注释符；
2  字符串的中间含有注释符；
其他 待定
*************************************/
static int vty_check_buf( char * str )
{
    char * pStr;
    if ( NULL == str )
    {
        VOS_ASSERT( 0 );
        return -1; /*编程有误*/
    }
    pStr = str;
    while ( ( '\t' == *pStr ) || ( ' ' == *pStr ) )  /*过滤掉空格和tab键*/
    {
        pStr++;
    }
    if ( ( '#' == *pStr ) || ( '!' == *pStr ) )
    {
        return 1; /*第一个字符为 注释符*/
    }
    if ( ( NULL != VOS_StrChr( pStr, '#' ) ) || ( NULL != VOS_StrChr( pStr, '!' ) ) )
    {
        return 2; /*中间含有注释符*/
    }
    return 0;
}
/* Describe matched command function. */

/** 
 * 命令提示
 * @param[in]   vty             当前会话
 */
static void vty_describe_command ( struct vty * vty )
{
    #define VTY_DEFAULT_WIDTH  80
    LONG ret;
    char *lowbuf;
    cl_vector vline;
    cl_vector describe = NULL;
    unsigned int i, width, desc_width;
    struct desc *desc, *desc_cr = NULL;

    /*检查输入的合法性*/
    if ( 1 == vty_check_buf( vty->buf ) )  /*检查字符串的第一个字母是否为注释符*/
    {
        vty->cp = vty->length = 0;
        vty_clear_buf ( vty );
        vty_out( vty, "%s", VTY_NEWLINE );
        vty_prompt( vty );
        return ;
    }

    lowbuf = VOS_StrDupToLower( vty->buf, MODULE_RPU_CLI );
    if ( lowbuf == NULL )
    {
        return ;
    }
    vline = cmd_make_strvec ( lowbuf ); /*lowbuf has't '?'*/

    /* In case of '> ?'. */
    if ( vline == NULL )
    {
        vline = cl_vector_init ( 1 );
        if ( vline == NULL )
        {
            VOS_Free( lowbuf );
            return ;
        }
        cl_vector_set ( vline, NULL );
    }
    else if ( vos_isspace ( ( int ) vty->buf[ vty->length - 1 ] ) )  /*there is input "space+?"*/
    {
        cl_vector_set ( vline, NULL );
    }
    vty_out ( vty, "%s", VTY_NEWLINE );
    describe = cmd_describe_command ( vline, vty, &ret );

    /* Ambiguous error. */
    switch ( ret )
    {
        case CMD_ERR_AMBIGUOUS:
            {
                vty_out ( vty, "%% Error, Ambiguous command.%s", VTY_NEWLINE );
                goto cl_vty_describe_command_end;
            }

        case CMD_ERR_NO_MATCH:
            {
                vty_out ( vty, "%% Error, There is no matched command.%s", VTY_NEWLINE );
                goto cl_vty_describe_command_end;
            }
    }

    if ( NULL == describe )/*这段代码应该放在这儿!*/
    {
        VOS_ASSERT(0);/*不应该走到此处*/
        vty_out ( vty, "%% Error, There is no matched command.%s", VTY_NEWLINE );
        goto cl_vty_describe_command_end;
    }
/*不需要再次排序*/
/*   qsort ( describe->index, describe->max, sizeof( void * ), cmp_desc );*/

    /* Get width of command string. */
    width = 0;
    for ( i = 0; i < cl_vector_max ( describe ); i++ )
    {
        if ( ( desc = cl_vector_slot ( describe, i ) ) != NULL )
        {
            unsigned int len;
            if ( desc->cmd[ 0 ] == '\0' )
            {
                continue;
            }
            len = VOS_StrLen ( desc->cmd );
            if ( desc->cmd[ 0 ] == '.' )
            {
                len--;
            }
            if ( width < len )
            {
                width = len;
            }
        }
    }

    /* Get width of description string. */
    if ( vty->width != 0 )
    {
        desc_width = vty->width - ( width + 6 );
    }
    else    /* 默认值为每行80个字符by liuyanjun 2002/10/15 */
    {
        desc_width = VTY_DEFAULT_WIDTH - ( width + 6 );
    }

    /* Print out description. */
    for ( i = 0; i < cl_vector_max ( describe ); i++ )
    {
        if ( ( desc = cl_vector_slot ( describe, i ) ) != NULL )
        {
            if ( desc->cmd[ 0 ] == '\0' )
            {
                continue;
            }
            if ( VOS_StrCmp ( desc->cmd, "<cr>" ) == 0 )
            {
                desc_cr = desc;
                continue;
            }
            if ( !desc->str )
            {
                vty_out ( vty, "  %-s%s",
                          desc->cmd[ 0 ] == '.' ? desc->cmd + 1 : desc->cmd, VTY_NEWLINE );
            }
            else if ( desc_width >= VOS_StrLen ( desc->str ) )
            {
                vty_out ( vty, "  %-*s  %s%s", width,
                          desc->cmd[ 0 ] == '.' ? desc->cmd + 1 : desc->cmd, desc->str, VTY_NEWLINE );
            }
            else
            {
                vty_describe_fold ( vty, width, desc_width, desc );
            }
        }
    }
    if ( ( desc = desc_cr ) != NULL )
    {
        if ( !desc->str )
        {
            vty_out ( vty, "  %-s%s",
                      desc->cmd[ 0 ] == '.' ? desc->cmd + 1 : desc->cmd,
                      VTY_NEWLINE );
        }
        else if ( desc_width >= VOS_StrLen ( desc->str ) )
        {
            vty_out ( vty, "  %-*s  %s%s", width,
                      desc->cmd[ 0 ] == '.' ? desc->cmd + 1 : desc->cmd,
                      desc->str, VTY_NEWLINE );
        }
        else
        {
            vty_describe_fold ( vty, width, desc_width, desc );
        }
    }

    cl_vector_free( describe );
cl_vty_describe_command_end:
    VOS_Free( lowbuf );
    cmd_free_strvec ( vline );
    vty_prompt ( vty );
    vty_redraw_line ( vty );

    return ;
}

int vty_QpmzCmd(struct vty *vty, CHAR *buf)
{
#ifdef _CL_DEBUG_HIDDEN_CMD_
        if ( ( VOS_StrCmp( buf, ( _CL_ENTER_HIDDEN_CMD_ ) ) == 0 ) && ( vty->node != VOS_CLI_DEBUG_HIDDEN_NODE ) )
        {
            vty->prev_node = vty->node;
            vty->node = VOS_CLI_DEBUG_HIDDEN_NODE ;
            if ( vty->t_timeout )
            {
                cl_lib_thread_cancel ( vty->t_timeout );
            }
            vty->t_timeout = NULL;
            return CMD_SUCCESS;
        }

#endif
    return CMD_ERR_NO_MATCH;
}

/* Command execution over the vty interface. */
int vty_command ( struct vty * vty, char * buf )
{
	/*extern ULONG IfIndexSwitch( ULONG ifindex );*/

    int ret;
    cl_vector vline;
    struct cmd_element *cmd = NULL ;
    

    vty_cmd_check_qpmz_openshell(vty);

    ret = vty_QpmzCmd(vty, buf);
    if(ret == CMD_SUCCESS) return ret;

    /* Split readline string up into the cl_vector */
    vline = cmd_make_strvec ( buf );
    if ( vline == NULL )
    {
        return CMD_SUCCESS;
    }

#if 0
    {
        char *arr[10]={NULL};
        int i = 0;
        for(i = 0;i < 10 && i <vline->max;i++)
        {
            arr[i] = (vline->index[i]);
        }

        i++;
        i++;
    }
#endif
    ret = cmd_execute_command ( vline, vty, &cmd );

    if ( ret != CMD_SUCCESS )
    {
        {
            switch ( ret )
            {
                case CMD_WARNING:
                    {
                        {
                            vty_out ( vty, "%% Error, Warning...%s", VTY_NEWLINE );
                        }
                    }
                    break;

                case CMD_ERR_AMBIGUOUS:
                    {
                        vty_out ( vty, "%% Error, Ambiguous command.%s", VTY_NEWLINE );
                    }
                    break;

                case CMD_ERR_NO_MATCH:
                    {
                        vty_out ( vty, "%% Error, Unknown command.%s", VTY_NEWLINE );
                    }
                    break;

                case CMD_ERR_INCOMPLETE:
                    {
                        vty_out ( vty, "%% Error, Command incomplete.%s", VTY_NEWLINE );
                    }
                    break;
            }
        }
    }

    cmd_free_strvec ( vline );

    return ret;
}

/************************************************
 
vty control function
 
vty_read : read input from telnet or console 
vty_flush : Flush vty buffer to the telnet or console. 
vty_execute : excute command 
vty_timeout : do something when  vty timeout
vty_config_lock
vty_config_unlok: lock for config mode.
 
****************************************************/ 
/* Create new vty structure. */

/** 
 * 创建vty，并启动一个会话任务
 * @param[in]   vty_sock     输入输出fd
 * @param[in]   su           socket
 * @return      成功返回 VOS_OK,失败返回其他
 */
struct vty *vty_create ( int vty_sock, union cl_lib_sockunion * su )
{
    struct vty *vty = NULL;
    struct vty *v = NULL;
    char temp_str[ 50 ];
    VOS_HANDLE task_id;
    static int task_index;
    ULONG ulArgs[ VOS_MAX_TASK_ARGS ];
    int i;
    int connection_count;

    /* Allocate new vty structure and set up default values. */
    vty = vty_new ();
    if ( vty == NULL )
        return 0;

//    vty->g_ulSendMessageSequenceNo = 0;

    vty->fd = vty_sock;
    vty->type = VTY_TERM;

    /* added  for syslog module */
    //vty->monitor_conf.cLog_Type_Count = 0;//VOS_Get_Syslog_Type_Count();

    if ( su->sa.sa_family == AF_TTYS )
    {
        vty->conn_type = _CL_VTY_CONSOLE_;
        /*      vty->address = "Console"; */
        /*      vty->address = cl_console_port; */
        vty->address = _CL_CONSOLE_NAME_;
    }


#ifdef _MN_HAVE_TELNET_
#ifdef _MN_HAVE_TELNET_V6_
    else if ( su->sa.sa_family == AF_INET6 )
    {
        vty->conn_type = _CL_VTY_TELNET_V6_;
        vty->address = cl_lib_sockunion_su2str ( su ,MODULE_RPU_CLI);
    }
#endif /* #ifdef _MN_HAVE_TELNET_V6_ */

    else if ( su->sa.sa_family == AF_INET )
    {
        vty->conn_type = _CL_VTY_TELNET_;
        vty->address = cl_lib_sockunion_su2str ( su ,MODULE_RPU_CLI);
    }
#endif /* #ifdef _MN_HAVE_TELNET_ */


    vty->user_name = VOS_StrDup ( vty->address ,MODULE_RPU_CLI );
    vty->context_data = NULL;
    vty->context_data_need_free = 0;



    vty->node = VOS_CLI_VIEW_NODE;
    VOS_MemZero( vty->vty_cmd_prompt, 64 );
    vty->cp = 0;
    vty_clear_buf ( vty );
    vty->length = 0;
    VOS_MemZero ( ( char * ) vty->hist, sizeof ( vty->hist ) );
    vty->hp = 0;
    vty->hindex = 0;
    cl_vector_set_index ( cl_vtyvec_master, vty_sock, vty );
    cl_vty_session_count++;
    vty->status = VTY_NORMAL;

    vty->v_timeout = _CL_VTY_TIMEOUT_LOGIN_;
    vty->lines = _CL_VTY_TERMLENGTH_DEFAULT_;
    vty->iac = 0;
    vty->iac_sb_in_progress = 0;
    vty->sb_buffer = buffer_new ( BUFFER_STRING, 1024 );
    vty->config = 0;
#ifdef _CL_CMD_PTY_RELAY_
    vty->relay_keep_alive_status = 0;
    vty->cl_vty_in_relay = 0 ;
    vty->cl_vty_relay_type = 0;
    vty->cl_relay_sb_buffer = buffer_new( BUFFER_STRING, 1024 );
#endif
    connection_count = 0;
    for ( i = 0; i < ( int ) cl_vector_max ( cl_vtyvec_master ); i++ )
    {
        if (
            ( ( v = cl_vector_slot ( cl_vtyvec_master, i ) ) != NULL ) &&
           ( ( v->conn_type == _CL_VTY_TELNET_ )|| ( vty->conn_type == _CL_VTY_TELNET_V6_))        /*此处 是否有效?*/

        )
        {
            connection_count++;
        }
    }
    /*    task_index = connection_count;*/
    if (
        (( vty->conn_type == _CL_VTY_TELNET_ )||( vty->conn_type == _CL_VTY_TELNET_V6_ )) &&   /*避免使console也被限制住*/
        ( connection_count > _CL_MAX_VTY_TELNET_COUNT_ ) &&
        ( cl_vty_user_limit )
    )
    {
        vty_out ( vty, "%% Error, telnet connection counts have reached max_count(%d) ,please try later.%s", _CL_MAX_VTY_TELNET_COUNT_, VTY_NEWLINE );

        /* Close connection. */
        vty->status = VTY_CLOSE;
        vty_task_exit ( vty );
        return 0;
    }

#if 1
    /* Setting up terminal.  Only socket connection telnet may
    need this */
    if (( vty->conn_type == _CL_VTY_TELNET_ )||( vty->conn_type == _CL_VTY_TELNET_V6_ ))
    {
        /*
        vty_epon(vty);
        */
        vty_will_echo ( vty );
        vty_will_suppress_go_ahead ( vty );
        vty_dont_linemode ( vty );
        vty_do_window_size ( vty );
        /*   vty_dont_lflow_ahead (vty); */
    }
#endif

    if (( vty->conn_type == _CL_VTY_CONSOLE_ ) || ( vty->conn_type == _CL_VTY_TELNET_ )|| ( vty->conn_type == _CL_VTY_TELNET_V6_ ))
    {
        vty_hello ( vty );
    }

    vty_prompt ( vty );

    vty->sub_thread_master = cl_lib_thread_make_master ();

    if ( vty->conn_type == _CL_VTY_CONSOLE_ )
    {
        ulArgs[ 0 ] = ( unsigned long ) vty;
        task_id = VOS_TaskCreateEx( "tCliConsole", vty_sub_main, VOS_CONSOLE_TASK_PRIORITY,
                                    VOS_DEFAULT_STACKSIZE, ulArgs );
    }
    else
    {
        task_index += 1;
        VOS_Sprintf( temp_str, "tCli_%d", task_index );
        ulArgs[ 0 ] = ( unsigned long ) vty;
        task_id = VOS_TaskCreateEx( temp_str, vty_sub_main, VOS_CONSOLE_TASK_PRIORITY,
                                    VOS_DEFAULT_STACKSIZE, ulArgs );
    }

    if ( task_id == NULL )
    {
        vty->status = VTY_CLOSE;
        vty_task_exit ( vty );
        return 0;
    }
    vty->sub_task_id = task_id;

	VOS_Printf("creat vty task:%ld\n", vty->sub_task_id);
#ifdef _CL_CMD_PTY_RELAY_
    vty->cl_relay_fd_sem = VOS_SemBCreate(VOS_SEM_Q_FIFO, VOS_SEM_FULL);
    vty->cl_relay_fd_close_sync_sem = VOS_SemBCreate(VOS_SEM_Q_FIFO, VOS_SEM_EMPTY);
#endif

    /* Add read/write thread. */
    vty_event ( VTY_WRITE, vty_sock, vty );
    vty_event ( VTY_READ, vty_sock, vty );
	LONG vtyTimeout = 0;
	vty_timeout_config(vty,0,&vtyTimeout);

    return vty;
}


/* 增加引用计数 */
void vty_get( struct vty * vty )
{
    int ncount = 0;
    struct vty * pvty = vty ;

    if ( vty == NULL )
    {
        return ;
    }

    vos_cli_lock() ;
    ncount = ++vty->ncount ;
    vos_cli_unlock() ;

    #ifdef PLAT_MODULE_SYSLOG
    VOS_log( MODULE_RPU_CLI , SYSLOG_INFO , "Get vty session %d ncount = %d", ( int ) pvty->fd , ncount );
    #endif
    /* debug_out( LOG_TYPE_CLI , "Get vty session %d ncount = %d", (int)pvty->fd , ncount ); */

}

/* 减小引用计数, 当引用计数为0的时候调用vty_close释放内存 */
void vty_put( struct vty * vty )
{
    if ( vty == NULL )
    {
        VOS_ASSERT( 0 );
        return ;
    }

    vos_cli_lock();
    vty->ncount--;
    if ( vty->ncount <= 0 )
    {
        vty_close( vty);
    }
    vos_cli_unlock();

    return ;
}


/* reboot的时候调用，执行最少操作 */
void vty_reboot( struct vty * vty )
{
    int vty_conn_type;
    vty_conn_type = vty->conn_type;

    /* 把socket 设置成为 no block  */
#ifdef _MN_HAVE_TELNET_

    if ( ((vty_conn_type == _CL_VTY_TELNET_)||(vty_conn_type == _CL_VTY_TELNET_V6_)) && vty->fd > 0 )
    {
        int opt = 1;
        ioctl( vty->fd, FIONBIO, (void *)&opt );
    }
#endif  /* #ifdef _MN_HAVE_TELNET_    */
    if ( _CL_VTY_CONSOLE_ == vty_conn_type )
    {
        int opt = 0 ;
        ioctl_realSystem( vty->fd, FIONBIO, (void *) &opt ); /* vxworks FIONBIO = 16*/
    }

    if ( ( vty->fd != _CL_FLASH_FD_ ) && ( vty->fd ) )
    {
        /*
        vty_out(vty,"[%d]",vty->fd);
        */

        {
            /*vty_buffer_reset( vty) ;*/
            vty_out( vty, "\r\nDisconnected.%s", VTY_NEWLINE );
            /*vty_out( vty, "Thanks for using GW Technologies product.%s", VTY_NEWLINE );*/	
  	     vty_out( vty, "Thanks for using %s product.%s", "typesdb_product_corporation_short_name()", VTY_NEWLINE );
            /*vty_out( vty, "  Bye!%s", VTY_NEWLINE );*/
            vty_flush_all( vty ) ;
        }
    }

    if (( vty_conn_type == _CL_VTY_TELNET_ )||(vty_conn_type == _CL_VTY_TELNET_V6_))
    {
        /* Close socket */
        if ( vty->fd > 0 )
        {
            cl_close ( vty->fd );
            vty->fd = 0 ;
        }
    }

    return ;
}

/* 结束与vty相关的任务,并且调用vty_put把vty计数减1 */

/** 
 * 关闭vty 任务
 * @param[in]   vty     vty
 */
void vty_task_exit( struct vty * vty )
{
    int i;
    int vty_conn_type;
    int vty_old_fd;
    VOS_HANDLE sub_task_id = NULL ;
    struct cl_lib_thread_master *sub_thread_master = NULL ;
//    int timeout = 0;

    if ( NULL == vty )
    {
        VOS_ASSERT( 0 );
        return ;
    }
#ifdef _CL_CMD_PTY_RELAY_
    if((vty->cl_vty_in_relay  == 1) && 
		((vty->cl_vty_relay_type == 2)||(vty->cl_vty_relay_type == 3)))
    {
       vty->cl_vty_relay_exit = 1;

	   VOS_TaskDelay(VOS_TICK_SECOND + 10);
    }
#endif
    #ifdef PLAT_MODULE_SYSLOG
    VOS_log (MODULE_RPU_CLI,SYSLOG_CRIT, "User %s logged out from %s\r\n",vty->user_name,vty->address);
    #endif


    /* 在主任务中添加对命令行多任务的监视,包括串口的子任务和其他的telnet的子任务
       有子任务死掉或者没有响应,就杀掉这个子任务,如果是串口就重新启动串口的描述符
       用于再次登陆 by liuyanjun 2002/07/06 
       但此时忽略了串口和telnet的idletimeOut 情况,在idletimeout时这个命令行的子任务已经被删掉了
       所以在idletimeout时加入清除任务状态的函数调用2002/09/02 */
    cliClearActiveTaskWhenVtyClose();


    vty_conn_type = vty->conn_type;
    vty_old_fd = vty->fd;

    if ( vty->debug_monitor == 1 )
    {
        vty->debug_monitor = 0;
        cl_monitor_vty_count --;
    }


    /* 把socket 设置成为 no block  */
#ifdef _MN_HAVE_TELNET_
    if ((( vty_conn_type == _CL_VTY_TELNET_)||(vty_conn_type == _CL_VTY_TELNET_V6_)) && vty->fd > 0 )
    {
        int opt = 1;
        ioctl( vty->fd, FIONBIO, (void *)&opt );
    }
#endif /* #ifdef _MN_HAVE_TELNET_  */
    if ( _CL_VTY_CONSOLE_ == vty->fd )
    {
        int opt = 0 ;
        ioctl_realSystem( vty->fd, FIONBIO, (void *)&opt ); /* vxworks FIONBIO = 16*/
    }

#if 0
    if ( vty->ping_taskid != NULL )
    {
            if (VOS_TaskHandleVerify((VOS_HANDLE) vty->ping_taskid ) == TRUE) /*判断任务是否非法，避免可能出现的问题*/
            {
                timeout = 0;
                while((vty->ping_task_ready) != 1)
                {
                    VOS_TaskDelay(2);
                    if(timeout++ > 6*VOS_TICK_SECOND) /* modi by suxq 2007 -05-21 */
                       break; 
                }
                kill(((VOS_TASKCB *)vty->ping_taskid)->ulOSId,SIGINT);

                timeout = 0;
                while(vty->ping_taskid)
                {
                    VOS_TaskDelay(1);
                    if(timeout++ > 6*VOS_TICK_SECOND) /* modi by suxq 2007 -05-21 */
                       break; 
                }

        }

        vty->ping_taskid = NULL;
    }
#endif

#ifdef _CL_CMD_PTY_RELAY_
    /* close telnet relay */
    if ( vty->cl_telnet_taskid != NULL )
    {
        if(vty->cl_vty_relay_type == 2)  cl_pty_relay_close(vty); /* 关闭PTY relay 应该走不到此*/
		else if(vty->cl_vty_relay_type == 3)  cl_cdp_pty_client_close(vty); 
        else
        {
            timeout = 0;
            
            if ( VOS_TaskHandleVerify( ( VOS_HANDLE ) vty->cl_telnet_taskid ) == VOS_TRUE )  /*判断任务是否非法，避免可能出现的问题*/
            {
	           	VOS_Printf("try to kill vty task:%ld\n", vty->cl_telnet_taskid);
                VOS_TaskTerminate(( VOS_HANDLE )(vty->cl_telnet_taskid), 1);
                while ( vty->cl_telnet_taskid )
                {
                    VOS_TaskDelay( 10 );
                    if ( timeout++ > 6 * VOS_TICK_SECOND ) /* modi by suxq 2007 -05-21 */
                        break;
                    }
            }
        }
    }

#endif /* #ifdef _CL_CMD_PTY_RELAY_ */

    if ( ( vty->fd != _CL_FLASH_FD_ ) && ( vty->fd ) )
    {
        /*
        vty_out(vty,"[%d]",vty->fd);
        */

        {
            /*vty_buffer_reset( vty) ;*/
            vty_out( vty, "\r\nDisconnected.%s", VTY_NEWLINE );
  	     /*vty_out( vty, "Thanks for using GW Technologies product.%s", VTY_NEWLINE );*/
  	     vty_out( vty, "Thanks for using %s product.%s", VOS_get_product_name(), VTY_NEWLINE );
            /*vty_out( vty, "  Bye!%s", VTY_NEWLINE );*/
            vty_flush_all( vty ) ;
        }
    }

    vty_buffer_reset( vty ) ;

    /* Free command history. */
    for ( i = 0; i < VTY_MAXHIST; i++ )
    {
        if ( vty->hist[ i ] )
        {
            VOS_Free ( vty->hist[ i ] );
            vty->hist[ i ] = NULL;
        }
    }

    /* Unset cl_vector. */
    if ( vty->fd < ( int ) cl_vtyvec_master->alloced &&
            cl_vtyvec_master->index[ vty->fd ] != NULL )
    {
        cl_vty_session_count -- ;
    }
    cl_vector_unset ( cl_vtyvec_master, vty->fd );

    /* if it's a socket connect then close socket fd */
    if ( vty_conn_type != _CL_VTY_CONSOLE_ )
    {
        /* Close socket */
        if ( vty->fd > 0 )
        {
            cl_close ( vty->fd );
            vty->fd = 0 ;
        }
        if ( vty->address )
        {
            VOS_Free ( vty->address );
            vty->address = NULL;
        }
    }

    /* Cancel threads. */
    if ( vty->t_read )
    {
        cl_lib_thread_cancel ( vty->t_read );

        vty->t_read = NULL ;

    }
    if ( vty->t_write )
    {
        cl_lib_thread_cancel ( vty->t_write );

        vty->t_write = NULL ;
    }
    if ( vty->t_timeout )
    {
        cl_lib_thread_cancel ( vty->t_timeout );
        vty->t_timeout = NULL ;
    }

    /* Check configure. */
      /* if vty is not in config node before exit, we should not call vty_config_unlock() */

    if ( 1 == vty->config )
        vty_config_unlock ( vty );

    /* if it's a console connection then restart serv */
    if ( ( vty_conn_type == _CL_VTY_CONSOLE_ ) )
    {
        cl_vty_serv_console ( vty_old_fd );
        
    };


    sub_task_id = vty->sub_task_id ;
    vty->sub_task_id = NULL ;
    sub_thread_master = vty->sub_thread_master ;
    vty->sub_thread_master = NULL;
    vty_put( vty );

    if ( sub_thread_master )
    {
        cl_lib_thread_destroy_master( sub_thread_master );
    }

    if ( sub_task_id != NULL )
    {
        if ( VOS_TaskHandleVerify( sub_task_id ) )
        {
        	/*增加延时，确保syslog模块已接收到vty发送的消息*/
        	VOS_TaskDelay(100);
            VOS_TaskTerminate( sub_task_id, 0 );
        }
    }


}

#ifdef _CL_CMD_PTY_RELAY_
int cl_relay_close( struct vty * vty )
{
    char *hostname;
    vos_cli_lock();
    if ( vty->t_relay_read )
    {
        cl_lib_thread_cancel( vty->t_relay_read );
        vty->t_relay_read = NULL;
    }

    if ( vty->t_relay_keep_alive_timeout )
    {
        cl_lib_thread_cancel( vty->t_relay_keep_alive_timeout );
        vty->t_relay_keep_alive_timeout = NULL;

    }
    if ( vty->cl_relay_thread_master )
    {
        cl_lib_thread_destroy_master( vty->cl_relay_thread_master );
        vty->cl_relay_thread_master = NULL;
    }

    /* relay exit */
    if ( vty->cl_relay_fd )
    {
#if 0
        cl_write( vty->cl_relay_fd, "exit\r\n", 6 );
#endif

       VOS_SemTake(vty->cl_relay_fd_sem, VOS_WAIT_FOREVER);
       cl_close ( vty->cl_relay_fd );
        vty->cl_relay_fd = 0;
	VOS_SemGive(vty->cl_relay_fd_sem);
    }

	vty->cl_vty_relay_type = 0;
    vty->relay_keep_alive_status = 1;

    vty_direct_out( vty, "\r\n\r\nConnection closed by foreign host.%s%s", VTY_NEWLINE, VTY_NEWLINE );

    if ( ( vty->type == VTY_TERM )
            && ( vty->status != VTY_CLOSE ) )
    {
        hostname = VOS_get_hostname();
        vty_direct_out ( vty, cmd_prompt ( vty->node ), hostname );
    }
    vos_cli_unlock();
    return 1;

}
#endif

/* Close vty interface. 释放内存 */

/** 
 * 关闭vty
 * @param[in]   vty     vty
 * @return      成功返回 VOS_OK,失败返回其他
 */
void vty_close ( struct vty * vty)
{
    int loop = 0;
    vty_timeout_callback_func func = NULL;
    
    if ( vty == NULL )
    {
        return ;
    }
#ifdef _CL_CMD_PTY_RELAY_  
    if(vty->cl_vty_in_relay != 0)
    {
        vty->cl_vty_relay_exit = 1;
        VOS_TaskDelay(VOS_TICK_SECOND + 10);        
    }
#endif
    /* Close connection. */
    for(loop=0;loop<VTY_TIMEOUT_EVENT_MAX;loop++)
    {
        func = GetVtyTimeoutHandlerFunc(loop);
        if(func)
            (*(func))(vty);
    }
    

    /* OK free vty. */
    if ( vty->Sem_obuf != 0 )
    {
        VOS_SemDelete( vty->Sem_obuf );
        vty->Sem_obuf = 0 ;
    }

    buffer_free ( vty->obuf );
    vty->obuf = NULL;

    if(vty->context_data_need_free && vty->context_data)
    {
        VOS_Free(vty->context_data);
        vty->context_data = NULL;
        vty->context_data_need_free = 0;
    }

#ifdef _CL_CMD_PTY_RELAY_

    if ( vty->cl_vty_in_relay != 0 )
    {
        if(vty->cl_vty_relay_type == 2)    /* modi by suxq 2008-06-19 */
        {
            cl_pty_relay_close(vty);        
        }
		else if(vty->cl_vty_relay_type == 3)    /* modi by suxq 2008-06-19 */
        {
            cl_cdp_pty_client_close(vty);        
        }
		else
		{
            cl_relay_close( vty ); 
            vty->cl_vty_in_relay = 0 ;
		}
    }
#endif /* #ifdef _CL_CMD_PTY_RELAY_ */

    /* Free SB buffer. */
    if ( vty->sb_buffer )
    {
        buffer_free ( vty->sb_buffer );
        vty->sb_buffer = NULL;
    }
#ifdef _CL_CMD_PTY_RELAY_  
    {
        if ( vty->cl_relay_sb_buffer )
        buffer_free( vty->cl_relay_sb_buffer );
        vty->cl_relay_sb_buffer = NULL;
    }
#endif
    if ( vty->buf )
    {
        VOS_Free ( vty->buf );
        vty->buf = NULL;
    }

    if ( vty->user_name )
    {
        VOS_Free( vty->user_name );
        vty->user_name = NULL;
    }


    /* Release the semaphore used for PTY */
#ifdef _CL_CMD_PTY_RELAY_  
    if (vty->cl_relay_fd_sem != 0)
	VOS_SemDelete(vty->cl_relay_fd_sem);
    if (vty->cl_relay_fd_close_sync_sem != 0)
	VOS_SemDelete(vty->cl_relay_fd_close_sync_sem);
#endif	
    VOS_Free ( vty );
    vty = NULL;


    return ;
}


void cl_vty_confirm_action( struct vty * vty, char * buf )
{
    switch ( buf[ 0 ] )
    {
        case 'y':
        case 'Y':
            {
                vty_direct_out( vty, "%c%s", buf[ 0 ], VTY_NEWLINE );
                if ( vty->action_func )
                {
                    vty->action_func(vty);
                }
                vty_exit_node(vty);
                vty_prompt( vty );

            }
            break;

        case 'n':
        case 'N':
            {
                vty_direct_out( vty, "%c%s", buf[ 0 ], VTY_NEWLINE );
                if ( vty->no_action_func )
                {
                    vty->no_action_func(vty);
                }
                vty_exit_node(vty);
                vty_prompt( vty );
            }
            break;

        default:
            /* remmed by liwei056@2011-9-19 for D13446 */
            /* HTT的Telnet会先进入此分支，才进入上面的分支 */
            /* windows命令行的Telnet不会进入此分支 */
            vty_direct_out(vty, "\r\nPlease confirm [Y/N]: ");
            break;
    }

    return ;
}

/* Execute current command line. */
int vty_execute ( struct vty * vty )
{
    int ret;
    ret = CMD_SUCCESS;
    switch ( vty->node )
    {
        case VOS_CLI_CONFIRM_ACTION_NODE:
            break;

        default:
            {
				
                ret = vty_command ( vty, vty->buf );
				
                if ( vty->type == VTY_TERM )
                {
                    vty_hist_add ( vty );
                }
            }
            break;
    }

    /* Clear command line buffer. */
    vty->cp = vty->length = 0;
    vty_clear_buf ( vty );

    if ( ( vty->status != VTY_CLOSE )
            && ( vty->status != VTY_START )
            && ( vty->status != VTY_CONTINUE )
            && ( vty->node != VOS_CLI_CONFIRM_ACTION_NODE )
            && ( vty->frozen == 0 )
#ifdef _CL_CMD_PTY_RELAY_  
            && ( vty->cl_vty_in_relay != 1 ) 
#endif
            )
    {
        vty_prompt ( vty );
    }

    return ret;
}



#if 0  /* del :采用vty->cl_vty_exit 来控制，避免并发访问  */
static int vty_exit( struct cl_lib_thread * thread )
{
    /*int vty_sock = cl_lib_THREAD_FD (thread);*/
    struct vty *vty = cl_lib_THREAD_ARG ( thread );
    vty->t_event = NULL;
    vty_task_exit( vty );
    return 0;
}
#endif

static void vty_ExecuteCmd(struct vty *vty)
{
//    if ( vty->stFtpPara.chReturnShellNode )
//    {
//        if ( VOS_StrCmp( vty->buf, "ftp" ) == 0 )
//            VOS_Sprintf( vty->buf, "ftp 0.0.0.0" );
//    }
    {
        vty_out ( vty, "%s", VTY_NEWLINE );
        vty_execute ( vty );
    }
}
#define _CL_TARGET_SHELL_INPUT_USE_PIPE_

int vty_read_debug=0;
/*extern void cli_ping_finish( VOID *task_id );*/
static int vty_read ( struct cl_lib_thread * thread )
{
    int i = 0;
    int ret = 0;
    int nbytes;
    unsigned char buf[ VTY_READ_BUFSIZ ]={0};

    int vty_sock = cl_lib_THREAD_FD ( thread );
    struct vty *vty = cl_lib_THREAD_ARG ( thread );
    vty->t_read = NULL;

    /* Read raw data from socket */
    nbytes = cl_read ( vty->fd, ( char * ) buf, VTY_READ_BUFSIZ );

    if ( nbytes <= 0 )
    {
        vty->status = VTY_CLOSE;
        vty_task_exit ( vty );
        return 0;
    }
#if 1
	else
		{	
			if(vty_read_debug)
			{
			for(i=0;i<nbytes;i++)
				VOS_Printf("0x%02x\r\n",buf[i]);
			}
		}
#endif

#ifdef _DEBUG_VERSION_UNUSE_
    if ( buf[ 0 ] == CONTROL ( 'R' ) )
    {
        reset();
    }
#endif


#ifdef _CL_CMD_PTY_RELAY_

    if ( vty->cl_vty_in_relay != 0 && vty->cl_vty_relay_exit != 1 )
    {
        if ( buf[ 0 ] == CONTROL ( 'Q' ) )
        {
            #ifdef OS_LINUX
            #else
 			vty->cl_vty_relay_exit = 1 ;
			if(vty->cl_vty_relay_type == 2)    /* modi by suxq 2008-06-19 */
            {
				cl_pty_relay_close(vty);       /*暂不用调用*/
            }
			else if(vty->cl_vty_relay_type == 3)    /* modi by suxq 2008-06-19 */
            {
				cl_cdp_pty_client_close(vty);       /*暂不用调用*/
            }
			
			else
            {
            if ( vty->cl_vty_in_relay == 1 )
            {
                if ( vty->cl_telnet_taskid != NULL )
                {
                    if ( VOS_TaskTerminate( ( VOS_HANDLE )vty->cl_telnet_taskid, 1 ) != VOS_OK )
                    {
                        VOS_ASSERT( 0 );
                    }
                }
                else
                {
                    VOS_ASSERT( 0 );
                }
            }
            else
            {
                /* kill telent or telnet relay */
                VOS_SemTake( vty->sub_thread_master->ulSemKill, WAIT_FOREVER );
                /* 先向relay的任务发消息,使协议栈的阻塞函数退出*/
                if ( vty->cl_telnet_taskid != NULL )
                {
                    if ( VOS_TaskTerminate( ( VOS_HANDLE )vty->cl_telnet_taskid, 1 ) != VOS_OK )
                    {
                        VOS_ASSERT( 0 );
                    }
                }
                else
                {
                    VOS_ASSERT( 0 );
                }

                /* vty_event(VTY_TELNET_RELAY_EXIT,vty->cl_relay_fd,vty); */

                VOS_SemGive( vty->sub_thread_master->ulSemKill );
            }
			}
            vty_buffer_reset( vty ) ;
            vty_event ( VTY_READ, vty_sock, vty );
            return 0;
            #endif
        }

        if( vty->cl_relay_fd )
        {
            if(vty->cl_vty_relay_type == 2) 
				 write(vty->cl_relay_fd, ( char * ) buf, nbytes );
			else if(vty->cl_vty_relay_type == 3)
			{
				if(buf[0]!=IAC)/*简单过滤一下*/
					VOS_Printf("cdp pty not support\r\n");//cl_v_cdp_pty_readvty_flushpty( vty, buf, nbytes );
			} 
            else cl_write ( vty->cl_relay_fd, ( char * ) buf, nbytes );
        }
        vty_event ( VTY_READ, vty_sock, vty );

        return 0;
    }

#if 0    /* del by suxq 2008-06-19 */
    if ( vty->cl_vty_in_relay == 1 )
    {
        if ( buf[ 0 ] == CONTROL ( 'C' ) )
        {}
        else
        {
            cl_write ( vty->cl_relay_fd, ( char * ) buf, nbytes );

            vty_event ( VTY_READ, vty_sock, vty );

            return 0;
        }

    }
#endif
#endif /* #ifdef _CL_CMD_PTY_RELAY_ */


    if ( vty->debug_monitor == 1 )
    {
        if ( buf[ 0 ] == CONTROL( 'D' ) )
        {
            vty->debug_monitor = 0;
            cl_monitor_vty_count --;
            vty_out( vty, "%s", VTY_NEWLINE );
            vty_prompt( vty );
            vty_event ( VTY_WRITE, vty_sock, vty );
            vty_event ( VTY_READ, vty_sock, vty );

            return 0;
        }
    }

    for ( i = 0; i < nbytes; i++ )
    {

        /* 每超过4个字节 ，delay 一下，防止大量输入，其他任务无法执行 */
        if ( i % 4 == 0 )
        {
            VOS_TaskDelay( 1 );  
        }

        if ( buf[ i ] == IAC )
        {
            if ( !vty->iac )
            {
                vty->iac = 1;
                continue;
            }
            else
            {
                vty->iac = 0;
            }
        }
        if ( vty->iac_sb_in_progress && !vty->iac )
        {
            buffer_putc ( vty->sb_buffer, buf[ i ] );
            continue;
        }
        if ( vty->iac )
        {
            /* In case of telnet command */
            ret = vty_telnet_option ( vty, buf + i, nbytes - i );
            vty->iac = 0;
            i += ret;
            if ( ( nbytes - i ) <= 1 )
            {
                vty_event ( VTY_READ_ALIVE, vty_sock, vty );
                return 0;
            }
            continue;
        }

        if ( vty->frozen != 0 )
        {
            continue;
        }

        if ( vty->status == VTY_MORE )
        {
            switch ( buf[ i ] )
            {
                case CONTROL ( 'C' ) :
                    {
                        /*        case 'q':
                        case 'Q':
                        */
                        vty_buffer_reset_out ( vty );
                    }
                    break;

                default:
                    {
                    }
                    break;
            }

            continue;
        }
#if 0

        if ( vty->ping_taskid != NULL )
        {

            if ( taskIdVerify( vty->ping_taskid ) == ERROR )
            {
                vty->ping_taskid = 0;
            }
            else


            {
                switch ( buf[ i ] )
                {
                    case CONTROL ( 'C' ) :
                        {
                            if ( vty->ping_taskid )
                            {
                                int times = 0;
                                while(vty->ping_task_ready != 1)
                                {
                                    VOS_TaskDelay(2 );  
                                    if((times++) > 6)
                                    {
                                        break;
                                    }
                                }
                                kill(((VOS_TASKCB *)vty->ping_taskid)->ulOSId,SIGINT);
                            }
                        }
                        break;

                    default:
                        break;
                }

                continue;
            }
        }
#endif

        /* Escape character. */
        if ( vty->escape == VTY_ESCAPE )
        {
            vty_escape_map ( buf[ i ], vty );
            continue;
        }

        /* Pre-escape status. */
        if ( vty->escape == VTY_PRE_ESCAPE )
        {
            switch ( buf[ i ] )
            {
                case '[':
                    {
                        vty->escape = VTY_ESCAPE;
                    }
                    break;

                case 'b':
                    {
                        vty_backward_word ( vty );
                        vty->escape = VTY_NORMAL;
                    }
                    break;

                case 'f':
                    {
                        vty_forward_word ( vty );
                        vty->escape = VTY_NORMAL;
                    }
                    break;

                case 'd':
                    {
                        vty_forward_kill_word ( vty );
                        vty->escape = VTY_NORMAL;
                    }
                    break;

                case CONTROL ( 'H' ) :
                            case 0x7f:
                        {
                            vty_backward_kill_word ( vty );
                            vty->escape = VTY_NORMAL;
                        }
                    break;

                default:
                    {
                        vty->escape = VTY_NORMAL;
                    }
                    break;
            }
            continue;
        }
        switch ( buf[ i ] )
        {
            case CONTROL ( 'A' ) :
                {
                    vty_beginning_of_line ( vty );
                }
                break;

            case CONTROL ( 'B' ) :
                {
                    vty_backward_char ( vty );
                }
                break;

            case CONTROL ( 'C' ) :
                {
                    vty_stop_input ( vty );
                }
                break;

            case CONTROL ( 'D' ) :
                {
                    vty_delete_char ( vty );
                }
                break;

            case CONTROL ( 'E' ) :
                {
                    vty_end_of_line ( vty );
                }
                break;

            case CONTROL ( 'F' ) :
                {
                    vty_forward_char ( vty );
                }
                break;

            case CONTROL ( 'H' ) :
                        case 0x7f:
                    {
                        vty_delete_backward_char ( vty );
                    }
                break;

            case CONTROL ( 'K' ) :
                {
                    vty_kill_line ( vty );
                }
                break;

            case CONTROL ( 'N' ) :
                {
                    vty_next_line ( vty );
                }
                break;

            case CONTROL ( 'P' ) :
                {
                    vty_previous_line ( vty );
                }
                break;

            case CONTROL ( 'T' ) :
                {
                    vty_transpose_chars ( vty );
                }
                break;

            case CONTROL ( 'U' ) :
                {
                    vty_kill_line_from_beginning ( vty );
                }
                break;

            case CONTROL ( 'W' ) :
                {
                    vty_backward_kill_word ( vty );
                }
                break;

            case CONTROL ( 'Z' ) :
                {
                    vty_end_config ( vty );
                }
                break;

            case '\n':
                {
                    if ( i > 0 )
                    {
                        if ( buf[ i - 1 ] == '\r' )
                        {
                            break;
                        }
                    }
                }
                vty_ExecuteCmd(vty);
                break;
                
            case '\r':
                vty_ExecuteCmd(vty);
                break;

            case '\t':
                {
                    vty_complete_command ( vty );
                }
                break;

            case '?':
                {
                    vty_describe_command ( vty );
                }
                break;

            case '\033':
                {
                    /********************************************************************
                    '[' is for unix and win2k like telnet client ,
                    'O' is for win9x like telnet client       
                    ********************************************************/
                    if ( i + 1 < nbytes && ( ( buf[ i + 1 ] == '[' ) || ( buf[ i + 1 ] == 'O' ) ) )
                    {
                        vty->escape = VTY_ESCAPE;
                        i++;
                    }
                    else
                    {
                        vty->escape = VTY_PRE_ESCAPE;
                    }
                }
                break;

            default:
                {
                    if ( vty->node == VOS_CLI_CONFIRM_ACTION_NODE )
                    {
                        cl_vty_confirm_action( vty, ( char * ) & ( buf[ i ] ) );

                        break;
                    }
                    if ( buf[ i ] > 31 && buf[ i ] < 127 )
                    {
                        vty_self_insert ( vty, buf[ i ] );
                    }
                }
                break;
        }
    }

    /* Check status. */
    if ( vty->status == VTY_CLOSE )
    {
        vty_task_exit ( vty );
        return 0;
    }

    if ( vty->frozen == 2 )
    {
        return 0;
    }
    if ( vty->frozen == 1 )
    {
        vty_event ( VTY_READ, vty_sock, vty );
        return 0;
    }

    if ( ( vty->status == VTY_IOREDIRECT )
#ifdef _CL_CMD_PTY_RELAY_  
        || ( vty->cl_vty_in_relay != 0 ) 
#endif
        )
    {
        vty_event ( VTY_READ, vty_sock, vty );
        return 0;
    }
    vty_event ( VTY_WRITE, vty_sock, vty );
    vty_event ( VTY_READ, vty_sock, vty );

    return 0;

}

/* Flush vty buffer to the telnet or console. */
static int vty_flush ( struct cl_lib_thread * thread )
{
    int erase;
    int vty_sock = cl_lib_THREAD_FD ( thread );
    struct vty *vty = cl_lib_THREAD_ARG ( thread );
    vty->t_write = NULL;

    /* Tempolary disable read thread. */
    if ( vty->lines == 0 )
    {
        if ( vty->t_read )
        {
            cl_lib_thread_cancel ( vty->t_read );

            vty->t_read = NULL;
        }
    }

    if ( vty->status == VTY_MORE )
    {
        erase = 1;
    }
    else
    {
        erase = 0;
    }

    if ( vty->lines == 0 )
    {
        vty_flush_window ( vty, vty->width, 20, 0, 1 );
    }
    else
    {
        vty_flush_window ( vty , vty->width,
                           vty->lines >= 0 ? vty->lines : vty->height, erase,
                           0 );
    }

    if ( VOS_SemTake( vty->Sem_obuf, _CL_VTY_OUT_SEM_WAIT_TIME_ ) == VOS_OK )
    {
        if ( buffer_empty ( vty->obuf ) )
        {
            if ( vty->status == VTY_CLOSE )
            {
                VOS_SemGive( vty->Sem_obuf );
                vty_task_exit ( vty );
                return 0 ;
            }
            else
            {
                vty->status = VTY_NORMAL;
            }
            /* restore read thread */
            if ( vty->lines == 0 )
            {
                vty_event ( VTY_READ, vty_sock, vty );
            }
        }
        else
        {
            vty->status = VTY_MORE;
            if ( vty->lines == 0 )
            {
                vty_event ( VTY_WRITE, vty_sock, vty );
            }
        }
        VOS_SemGive( vty->Sem_obuf );
    }

    return 0;
}

ULONG g_ulDebugVtyConsoleOut = 1;
/* VTY standard output function. write strings to vty's output buffer */
int vty_out ( struct vty * vty, const char * format, ... )
{
    va_list args;
    int len = 0;

    char buf[ 1040 ]={0};


    /* vararg print */
    va_start ( args, format );
    len = sprintf(buf,"%s","\033[0m");
    /* this function maybe ocurr exception */
    len = VOS_Vsnprintf( &buf[len], 1023 , format, args );
    len = VOS_StrLen(buf);/*此处调用是因为上面的返回值可能不是真正的拷贝长度*/
    if ( len < 0 )
    {
        va_end ( args );
        return -1;
    }

    /*增加vty为null的打印，防止一些函数要求必须输入vty时，从shell下无发打印的问题*/
    if ( vty == NULL || vty->fd <= 0)
    {
        if(g_ulDebugVtyConsoleOut != 0)
          VOS_Printf("### %s", buf);
        va_end ( args );
        return 0;
    }

    if ( VOS_SemTake( vty->Sem_obuf, _CL_VTY_OUT_SEM_WAIT_TIME_ ) == VOS_OK )
    {
        buffer_write_vty_out ( vty->obuf, ( unsigned char * ) buf, len );
        VOS_SemGive( vty->Sem_obuf );
    }
    va_end ( args );

    return len;
}


/* 清空vty->obuf的内容，其他文件必须使用此函数reset vty 的obuf，防止并发访问 */
void vty_buffer_reset( struct vty * vty )
{
    if ( vty != NULL )
    {
        if ( VOS_SemTake ( vty->Sem_obuf , _CL_VTY_OUT_SEM_WAIT_TIME_ ) == VOS_OK )
        {
            if ( !buffer_empty ( vty->obuf ) )
            {
                buffer_reset( vty->obuf );
            }
            VOS_SemGive( vty->Sem_obuf );
        }
    }
    return ;
}

/* 输出vty->obuf的内容，调用时不需要取信号量 */
void vty_flush_all( struct vty * vty )   /* buffer_flush_all  */
{
    struct buffer *b = NULL ;
    struct buffer_data *head = NULL ;
    struct buffer_data *d;
    int i = 0 , ncount = 0;
    if(_CL_VTY_SSH_ == vty->conn_type)
    {
       return;
    }

    if ( vty != NULL && vty->fd > 0 )
    {
        VOS_SemTake( vty->Sem_obuf , _CL_VTY_OUT_SEM_WAIT_TIME_ ) ;
        if ( buffer_empty ( vty->obuf ) )
        {
            VOS_SemGive( vty->Sem_obuf );
            return ;
        }

        b = vty->obuf ;
        ncount = 0;
        head = b->head ;
        for ( d = b->head; d; d = d->next )
        {
            ncount ++ ;

            b->alloc -- ;
            b->length -= ( d->cp - d->sp );
            b->head = d->next ;
        }

        buffer_reset ( b );

        VOS_SemGive( vty->Sem_obuf ) ;

        for ( i = 0 ; i < ncount ; i++ )
        {
            d = head ;
            cl_write( vty->fd, ( char * ) ( d->data + d->sp ), ( int ) ( d->cp - d->sp ) );
            head = head->next ;
            buffer_data_free( d );
        }

    }

    return ;
}

static char cl_erase[] = { 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
						0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
						0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
						0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
						0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
						' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
						' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
						' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
						' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
						' ', ' ', ' ', ' ', ' ', ' ',
						0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
						0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
						0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
						0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
						0x08, 0x08, 0x08, 0x08, 0x08, 0x08
					  };

/* 输出vty 一个window 的内容，调用者不需要take信号量，必须采用此函数flush window ,
   不允许直接调用 buffer_flush_window */
int vty_flush_window( struct vty * vty , int width, int height, int erase_flag, int no_more )
{
    int i = 0 , ncount = 0 ;
    char *strArr = NULL ;
    int strArr_len = 0;
    struct buffer_data *head = NULL ;
    unsigned long head_sp = 0 ;
    struct buffer *b = NULL ;
    int fd = 0 ;
    unsigned long cp;
    unsigned long size;
    int lineno;
    int buf_empty = 0 ;

#if (OS_LINUX)

    char more[] = "\x1b\x5b\x37\x6d --Press any key to continue Ctrl+c to stop-- \x1b\x5b\x6d";
#else

    char more[] = " --Press any key to continue Ctrl+c to stop--";
#endif

    struct buffer_data *data;
    struct buffer_data *out;
    struct buffer_data *next;

    if(_CL_VTY_SSH_ == vty->conn_type)
    {
       return 0;
    }
    /* Previously print out is performed. */
    if ( vty == NULL || vty->fd <= 0 || vty->Sem_obuf == 0 )
    {
        return 0;
    }
    fd = vty->fd ;

    if ( erase_flag )
    {
        cl_write( fd, cl_erase, sizeof cl_erase );
    }

    VOS_SemTake( vty->Sem_obuf , _CL_VTY_OUT_SEM_WAIT_TIME_ ) ;
    b = vty->obuf ;
    {       /* buffer_flush_window */

        if ( height >= 2 )
        {
            height--;
        }

        /* We have to calculate how many bytes should be written. */
        lineno = 0;
        size = 0;
        if ( width == 0 )
            lineno++;

        for ( data = b->head; data; data = data->next )
        {
            cp = data->sp;

            while ( cp < data->cp )
            {
                if ( data->data[ cp ] == '\n' )
                {
                    lineno++;
                    if ( lineno == height )
                    {
                        cp++;
                        size++;
                        goto flush;
                    }
                }
                cp++;
                size++;
            }
        }
    }

    /* Write data to the file descriptor. */
flush:

    data = b->head;

    /* Output data. */
    if ( b->head )
    {
        head_sp = b->head->sp ;
    }
    else
    {
        VOS_SemGive( vty->Sem_obuf );
        return 0;
    }

    for ( data = b->head; data; data = data->next )
    {
        /* 如果输出的数据的大小或等于当前缓冲区中的数据的     */
        /* 长度，那么在当前缓冲区中取出size大小的数据来即可。 */
        /* data是最后一个需要取数据的缓冲区。                 */
        if ( size <= ( data->cp - data->sp ) )
        {

            /* cl_write(fd, (char *) (data->data + data->sp), size);   */
            data->sp += size;
            /* 如果当前缓冲区所有的数据已经取完，取下个缓冲区的数据。 */
            if ( data->sp == data->cp )
            {
                data = data->next;
            }
            else
            {
                strArr = VOS_Malloc ( size + 1 , MODULE_RPU_CLI );
                if ( strArr == NULL )  /* if no memory , free vty->obuf */
                {
                    buffer_reset( vty->obuf ) ;
                    VOS_SemGive( vty->Sem_obuf ) ;
                    return 0 ;
                }

                strArr_len = size ;
                VOS_MemZero( strArr, size + 1 ) ;
                VOS_MemCpy( strArr , ( char * ) ( data->data + data->sp - size ), size ) ;
            }

            break;
        }
        /* 否则，取出当前缓冲区中的所有数据。 */
        else
        {
            /* cl_write(fd, (char *) (data->data + data->sp), (int)(data->cp - data->sp));  */

            size -= ( data->cp - data->sp );
            data->sp = data->cp;
        }
    }

    /* Free printed buffer data. */
    /* 释放从链表头到data的所有的缓冲区。 */
    head = b->head ;
    ncount = 0;
    for ( out = b->head; out && out != data; out = next )
    {
        next = out->next;
        if ( next )
        {
            next->prev = NULL;
        }
        else
        {
            b->tail = next;
        }
        b->head = next;

        b->alloc--;
        b->length -= out->cp;

        ncount ++ ;
        /* buffer_data_free (out); */
    }

    buf_empty = buffer_empty ( b ) ;

    VOS_SemGive( vty->Sem_obuf ) ;

    out = head ;
    for ( i = 0 ; i < ncount ; i++ )
    {
        next = out->next ;
        if ( i == 0 )
        {
            cl_write( fd , ( char * ) ( out->data + head_sp ), ( int ) ( out->cp - head_sp ) );
        }
        else
        {
            cl_write( fd , ( char * ) ( out->data ), ( int ) ( out->cp ) );
        }
        buffer_data_free ( out );
        out = next ;
    }
    if ( strArr )
    {
        cl_write( fd , strArr , strArr_len );
        VOS_Free( strArr );
        strArr = NULL ;
    }

    /* In case of `more' display need. */
    if ( !buf_empty && !no_more )
    {
        cl_write( fd, ( char * ) more, sizeof more );
    }

    return 1;
}

/***********************************
VTY big output function. 
write long strings (>1k) to vty's output buffer 
**************************************/
int vty_big_out ( struct vty * vty, int maxlen, const char * format, ... )
{
    va_list args;
    int len = 0;

    char* buf;

    if ( vty == NULL )
    {
        return 0;
    }
    if ( vty->fd <= 0 )
    {
        return 0;
    }

    buf = ( char * ) VOS_Malloc( maxlen + 1 , MODULE_RPU_CLI );
    if ( buf == NULL )
    {
        return 0;
    }
    VOS_MemZero( buf, maxlen + 1 );

    /* vararg print */
    va_start ( args, format );
    len = VOS_Vsnprintf ( buf, maxlen + 1 , format, args );
    if ( len < 0 )
    {
        VOS_Free( buf );
        return -1;
    }

    if ( VOS_SemTake( vty->Sem_obuf, _CL_VTY_OUT_SEM_WAIT_TIME_ ) == VOS_OK )
    {
        buffer_write ( vty->obuf, ( unsigned char * ) buf, len );
        VOS_SemGive( vty->Sem_obuf );
    }
    va_end ( args );

    VOS_Free( buf );

    return len;
}

int vty_out_va (  struct vty * vty, const char * szFmt, va_list Args )
{
    int len = 0;

    char buf[ 1024 ];

    if ( vty == NULL )
    {
        return 0;
    }
    if ( vty->fd <= 0 )
    {
        return 0;
    }

    VOS_MemZero( buf, 1024 );
    /* this function maybe ocurr exception */
    len = VOS_Vsnprintf( buf, 1023 , szFmt, Args );
    len = VOS_StrLen(buf);/*此处调用是因为上面的返回值可能不是真正的拷贝长度*/
    if ( len < 0 )
    {
        return -1;
    }

    if ( VOS_SemTake( vty->Sem_obuf, _CL_VTY_OUT_SEM_WAIT_TIME_ ) == VOS_OK )
    {
        buffer_write_vty_out ( vty->obuf, ( unsigned char * ) buf, len );
        VOS_SemGive( vty->Sem_obuf );
    }

    return len;
}

/************************************
vty's output function , direct output strings to console or telnet fd 
****************************************/
int vty_direct_out ( struct vty * vty , const char * format, ... )
{
    va_list args;
    int len = 0;
    int vty_fd;
    int vty_status;

    char buf[ 1024 ];

    vos_cli_lock();
    /* 防止vty释放后还在使用 */
    if ( vty == NULL )
    {
        vos_cli_unlock();
        return 0;
    }
    vty_fd = vty->fd;
    vty_status = vty->status;
    vos_cli_unlock();

    if ( vty_fd <= 0 )
    {
        return 0;
    }
    

    if ( vty_status == VTY_CLOSE )
    {
        return 0;
    }
    VOS_MemZero( buf, 1024 );
    va_start ( args, format );
    if(_CL_VTY_SSH_ == vty->conn_type)
    {
        vty_out_va(vty, format,args);
    }
    else
    {
    len = VOS_Vsnprintf ( buf, sizeof buf, format, args );
    if ( len < 0 )
    {
        va_end ( args );
        return -1;
    }
    else
    {
        cl_write ( vty_fd, ( char * ) buf, len );
    }
    }
    va_end ( args );
    return len;
}


/* 向Console 台输出 */
int cl_vty_console_out( const char * format, ... )
{
    va_list args;
    char buf[ 1024 ];

    va_start ( args, format );
    VOS_MemZero( buf, 1024 );
    VOS_Vsnprintf ( buf, sizeof buf, format, args );
    va_end ( args );

    return VOS_Printf("%s",buf);
}


int cl_vty_assert_out( const char * format, ... )
{
    unsigned int i;
    struct vty *vty;
    va_list args;
    int len = 0;
    int opt = 1;

    char buf[ 1024 ];

    va_start ( args, format );
    VOS_MemZero( buf, 1024 );
    len = VOS_Vsnprintf ( buf, sizeof buf, format, args );
    va_end ( args );

    if ( len < 0 )
    {
        return 0;
    }

    {
      sys_console_buf_out( buf, len );
      return 1;
    }

      if ( VOS_Console_FD > 0 )  /*直接写串口*/
    {
        opt = 1 ;
        opt = ioctl_realSystem( VOS_Console_FD, FIONBIO, (void *)&opt ); /* vxworks FIONBIO = 16*/

        VOS_Printf( buf ) ;

        opt = 0 ;
        opt = ioctl_realSystem( VOS_Console_FD, FIONBIO, (void *)&opt ); /* vxworks FIONBIO = 16*/
    }

    if ( cl_vtyvec_master == NULL )       /*命令行还没有初始化*/
    {
        return 0;
    }
    /* assert 输出不能够是阻塞的*/
    for ( i = 0; i < cl_vector_max( cl_vtyvec_master ); i++ )
    {
        if ( ( vty = cl_vector_slot( cl_vtyvec_master, i ) ) != NULL )
        {
            vty_out(vty, buf);
        }
    }

    return 1;
}

int cl_vty_all_out( const char * format, ... )
{
    unsigned int i;
    struct vty *vty;
    va_list args;
    int len = 0;
    int opt = 1;    

    char buf[1024];

    /*int iTelnetWrite = 0;*/
    /* NO.001 ZHANGXINHUI
    if(g_stLoad_SessCtrl.enState != LOAD_SESS_CTRL_NONE)
      	return 0;
    */
    va_start (args, format);
    VOS_MemZero(buf, 1024);
    len = VOS_Vsnprintf (buf, sizeof buf, format, args);
    va_end (args);

    if (len < 0)
    {
        return 0;
    }

        {
          sys_console_buf_out( buf, len );
          return 1;
        }
   
    if (cl_vtyvec_master == NULL )      /*命令行还没有初始化*/
    {
        return 0;
    }  
    /* assert 输出不能够是阻塞的*/
    for (i = 0; i < cl_vector_max(cl_vtyvec_master); i++)
    {
        if ((vty = cl_vector_slot(cl_vtyvec_master,i)) != NULL)
        {
            {
			
                #ifdef _MN_HAVE_TELNET_     
                #if 1 /*在没有内存的情况下，协议栈的Ioctl 会阻塞 */
                /* 把socket 设置成为 no block  */
                if ((((vty->conn_type == _CL_VTY_TELNET_ )||(vty->conn_type == _CL_VTY_TELNET_V6_))&& vty->fd > 0))
                {
                    opt = 1;
                    opt = ioctl( vty->fd, FIONBIO, (void *)&opt );

                    cl_write(vty->fd , buf, len);
                    
                    opt = 0;
                    opt = ioctl( vty->fd, FIONBIO, (void *)&opt );
                }  
                #endif
                #endif /* #ifdef _MN_HAVE_TELNET_  */
			 }
        }    
    }

   if ( VOS_Console_FD > 0 )  /*直接写串口*/
    {
    	opt = 1 ;
        opt = ioctl_realSystem( VOS_Console_FD, FIONBIO, (void *)&opt );/* vxworks FIONBIO = 16*/    

        VOS_Printf(buf) ;

        opt = 0 ;
        opt = ioctl_realSystem( VOS_Console_FD, FIONBIO, (void *)&opt );/* vxworks FIONBIO = 16*/
    }

    return 1;
}
int cl_vty_telnet_out( const char * format, ... )
{
    unsigned int i;
    struct vty *vty;
    va_list args;
    int len = 0;

    char buf[1024];

    /*int iTelnetWrite = 0;*/
    /* NO.001 ZHANGXINHUI
    if(g_stLoad_SessCtrl.enState != LOAD_SESS_CTRL_NONE)
      	return 0;
    */
    va_start (args, format);
    VOS_MemZero(buf, 1024);
    len = VOS_Vsnprintf (buf, sizeof buf, format, args);
    va_end (args);

    if (len < 0)
    {
        return 0;
    }

        {
          sys_console_buf_out( buf, len );
          return 1;
        }
   
    if (cl_vtyvec_master == NULL )      /*命令行还没有初始化*/
    {
        return 0;
    }  
    /* assert 输出不能够是阻塞的*/
    for (i = 0; i < cl_vector_max(cl_vtyvec_master); i++)
    {
        if ((vty = cl_vector_slot(cl_vtyvec_master,i)) != NULL)
        {
            {
			
                #ifdef _MN_HAVE_TELNET_     
                #if 1 /*在没有内存的情况下，协议栈的Ioctl 会阻塞 */
                /* 把socket 设置成为 no block  */
                if ((vty->conn_type == _CL_VTY_TELNET_ && vty->fd > 0))
                {
                    int opt = 1;
                    
                    opt = 1;
                    opt = ioctl( vty->fd, FIONBIO, (void *)&opt );

                    cl_write(vty->fd , buf, len);
                    
                    opt = 0;
                    opt = ioctl( vty->fd, FIONBIO, (void *)&opt );
                }  
                #endif
                #endif /* #ifdef _MN_HAVE_TELNET_  */
		}
        }    
    }

    return 1;
}


/*
 *  当一个vty 删除 index 的时候，同步其他vty在此节点下的index ，
 *  并且使这些vty 从cur_node(如果在这个节点的话)退到pre_node ；如果vty不在cur_node 不执行任何操作。
 *  prompt : 给用户的提示,如: "\r\nVty index has been deleted.\r\n".为NULL 没有提示
 */
VOID cl_vty_syn_index( struct vty * v, void * index,
                       enum node_type cur_node, enum node_type pre_node, char * prompt )
{
    unsigned int i;
    struct vty *vty;

    for ( i = 0; i < cl_vector_max( cl_vtyvec_master ); i++ )
    {
        if ( ( vty = cl_vector_slot( cl_vtyvec_master, i ) ) != NULL )
        {
            if ( vty == v || vty->node != cur_node || vty->context_data != index )
            {
                continue ;
            }
            else
            {
                vty->node = pre_node ;

                /*clear index info ,it's now invalid*/
                vty->context_data = NULL ;

                vty_buffer_reset( vty ) ;
                if ( prompt != NULL )
                {
                    vty_out( vty , prompt ) ;
                }
                vty_prompt( vty ) ;
                vty_flush_all( vty ) ;
            }
        }
    }
}



/*
 *  当一个vty 删除 index 的时候，同步其他vty在此节点下的index ，
 *  并且使这些vty 从cur_node(如果在这个节点的话)退到pre_node ；如果vty不在cur_node 不执行任何操作。
 *  prompt : 给用户的提示,如: "\r\nVty index has been deleted.\r\n".为NULL 没有提示。
 *  与cl_vty_syn_index的区别是此函数仅在激发后输出prompt。
 
 *  终端同步函数，接口命令行专用。
 */
VOID cl_vty_syn_index_later( struct vty * v, void * index,
                             enum node_type cur_node, enum node_type pre_node, char * prompt )
{
    unsigned int i;
    struct vty *vty;

    for ( i = 0; i < cl_vector_max( cl_vtyvec_master ); i++ )
    {
        if ( ( vty = cl_vector_slot( cl_vtyvec_master, i ) ) != NULL )
        {
            if ( vty == v || vty->node != cur_node || vty->context_data != index )
            {
                continue ;
            }
            else
            {
                if( vty_exit_node(vty) == 0 )
            	{
                    vty->node = pre_node ;
                    vty->context_data = NULL ;
            	}
                vty_buffer_reset( vty ) ;
                if ( prompt != NULL )
                {
                    vty_out( vty , prompt ) ;
                }
                vty_prompt( vty ) ;
                /*vty_flush_all(vty) ;*/
            }
        }
    }
}


int cl_vty_monitor_out( const char * format, ... )
{
    unsigned int i;
    struct vty *vty;
    va_list args;
    int len = 0;

    char buf[ 1024 ];

    if ( cl_monitor_vty_count <= 0 )
    {
        return 0;
    }

    va_start (args, format);
    VOS_MemZero(buf, 1024);
    len = VOS_Vsnprintf (buf, sizeof buf, format, args);
    va_end (args);

    if (len < 0)
    {
        return 0;
    }

    for (i = 0; i < cl_vector_max(cl_vtyvec_master); i++)
    {
        if ((vty = cl_vector_slot(cl_vtyvec_master,i)) != NULL)
        {
            if ((vty->debug_monitor == 1)
#ifdef _CL_CMD_PTY_RELAY_  
                &&(vty->cl_relay_fd == 0)
#endif
                )
            {
                vty_direct_out(vty, (const char *) buf);
            }
        }    
    }

    return len;
}

#define PRINT_NEW_LINE_HEAD
#ifdef PRINT_NEW_LINE_HEAD
 #define NEW_LINE_HEAD "\n\r%%"
#endif

    /*print to all monitor*/
    int vty_out_2_all_login( const char * format, ... )
    {
      unsigned int i;
      struct vty *vty;
      va_list args;
      int len = 0;

      char buf[ 1024 ];
    va_start ( args, format );
    VOS_MemZero( buf, 1024 );
    len = VOS_Vsnprintf ( buf, sizeof buf, format, args );
    va_end ( args );

    if ( len < 0 )
    {
        return 0;
    }

    for ( i = 0; i < cl_vector_max( cl_vtyvec_master ); i++ )
    {
        if ( ( vty = cl_vector_slot( cl_vtyvec_master, i ) ) != NULL )
            {
              if ( vty->node >= VOS_CLI_VIEW_NODE )
                {
#ifdef PRINT_NEW_LINE_HEAD /*zsc added 2003-06-27*/
                  vty_out( vty, NEW_LINE_HEAD );
#endif

                  vty_out( vty, ( const char * ) buf );

                  if (1 )
                    {
                     /* char * hostname = mn_get_hostname();
                      vty_direct_out( vty, cmd_prompt ( vty->node ), hostname );*/
                    }
                  else
                    {
                      /* char *hostname = mn_get_hostname();
                     vty_direct_out( vty, vty->vty_cmd_prompt, hostname );*/
                    }
                  /*vty_out( vty, vty->buf );     deleted by wufei;  for D003953 */

                  vty_event(VTY_WRITE, vty->fd, vty);
            }
        }
    }

    return 1;
}

#if 1

/* 要使用vty_very_big_out 函数必须先调用此初始化函数 */
void vty_very_big_out_init( struct vty * vty )
{
    vty->veryBigOutLineCount = 1;
    vty->veryBigOutNoFirst = 0;
    cliCheckSetActiveTaskAfterSelect();
}

/*
0:成功执行
-1:出现一般性的错误,参数错误
-2:用户长时间没有输入,超时关闭打印的提示信息为
    Connection idle time out.
-3:从文件描述符中读数据出错
-4:用户按ctrl + c结束输出
*/
int vty_very_big_out ( struct vty * vty, const char * format, ... )
{
    int erase;
    va_list args;
    int len = 0;
    char buf[ 10240 ];
    int ret;

    int nbytes;
    unsigned char read_buf[ VTY_READ_BUFSIZ ];

    char erase_line[] = { 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
                          0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
                          0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
                          0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
                          0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
                          ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                          ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                          ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                          ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                          ' ', ' ', ' ', ' ', ' ', ' ',
                          0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
                          0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
                          0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
                          0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
                          0x08, 0x08, 0x08, 0x08, 0x08, 0x08
                        };


    if ( vty == NULL )
    {
        return -1;
    }
    if ( vty->fd <= 0 )
    {
        return -1;
    }

    /* vararg print */
    va_start ( args, format );
    VOS_MemZero( buf, sizeof(buf) );
    /* this function maybe ocurr exception */
    len = VOS_Vsnprintf( buf, sizeof(buf), format, args );
    if ( len < 0 )
    {
        va_end ( args );
        return -1;
    }

    if ( VOS_SemTake( vty->Sem_obuf, _CL_VTY_OUT_SEM_WAIT_TIME_ ) == VOS_OK )
    {
        buffer_write ( vty->obuf, ( unsigned char * ) buf, len );
        VOS_SemGive( vty->Sem_obuf );
    }
    va_end ( args );
    vty->veryBigOutLineCount++;
    /* 不满一屏,返回让其继续输出 */
    if ( vty->lines != 0 )
    {
        if ( vty->veryBigOutNoFirst == 0 )
        {
            if ( vty->veryBigOutLineCount < ( vty->lines - 2 ) )
            {
                return 0;
            }
        }
        else
        {
            if ( vty->veryBigOutLineCount < ( ( vty->lines - 2 ) + 1 ) )
            {
                return 0;
            }
        }
    }
    else
    {
        if ( vty->veryBigOutLineCount < ( _CL_VTY_TERMLENGTH_DEFAULT_ - 2 ) )
        {
            return 0;
        }
    }
    /* 满一屏,分页输出 */
    vty->veryBigOutLineCount = 1;

    if ( vty->status == VTY_MORE )
    {
        erase = 1;
    }
    else
    {
        erase = 0;
    }


    if ( vty->lines == 0 )
    {
        vty_flush_window ( vty, 0, _CL_VTY_TERMLENGTH_DEFAULT_, 0, 1 );

        VOS_SemTake( vty->Sem_obuf , _CL_VTY_OUT_SEM_WAIT_TIME_ ) ;
        if ( buffer_empty ( vty->obuf ) )
        {
        }
        else
        {
            buffer_reset ( vty->obuf );
        }
        VOS_SemGive( vty->Sem_obuf );

        return 0;
    }
    else
    {
        /* 为了输出anykey和ctrl + c */
        VOS_StrCpy( buf, "This is in vty_very_big_out.\r\n" );
        if ( VOS_SemTake( vty->Sem_obuf , _CL_VTY_OUT_SEM_WAIT_TIME_ ) == VOS_OK )
        {
            buffer_write ( vty->obuf, ( unsigned char * ) buf, VOS_StrLen( buf ) );
            VOS_SemGive( vty->Sem_obuf );
        }

        vty_flush_window ( vty, 0,
                           vty->lines >= 0 ? vty->lines : vty->height, erase,
                           0 );

    }

    vty->status = VTY_MORE;
    vty->veryBigOutNoFirst = 1;

    vty_buffer_reset( vty ) ;

    /* 在这里等待用户的输入,同时做超时和ctrl + c 处理 */
    if ( vty->fd)	
    {
        ret = cl_lib_console_select_fetch ( vty->fd, vty->v_timeout );
        /* 超时*/
        if ( ret == -1 )
        {
            /* 去掉上面的anykey和ctrl + c那一行 */
            if ( VOS_SemTake( vty->Sem_obuf, _CL_VTY_OUT_SEM_WAIT_TIME_ ) == VOS_OK )
            {
                buffer_write ( vty->obuf, ( unsigned char * ) erase_line, sizeof( erase_line ) );
                buffer_write ( vty->obuf, ( unsigned char * ) erase_line, sizeof( erase_line ) );
                VOS_SemGive( vty->Sem_obuf );
            }

            vty_out ( vty, "\r\n\r\n  %% Connection idle time out.%s", VTY_NEWLINE );
            vty->status = VTY_CLOSE;
            return -2;
        }

        nbytes = cl_read ( vty->fd, ( char * ) read_buf, VTY_READ_BUFSIZ );
        if ( nbytes <= 0 )
        {
            vty->status = VTY_CLOSE;
            return -3;
        }

        if ( read_buf[ 0 ] == CONTROL ( 'C' ) )
        {
            if ( VOS_SemTake( vty->Sem_obuf, _CL_VTY_OUT_SEM_WAIT_TIME_ ) == VOS_OK )
            {
                buffer_reset ( vty->obuf );
                VOS_SemGive( vty->Sem_obuf );
            }
            return -4;
        }
        else
        {
            return 0;
        }

    }

    return 0;
}
#endif

void cl_vty_monitor_count_inc( void )
{
    cl_monitor_vty_count++;
    return ;
}
void cl_vty_monitor_count_dec( void )
{
    cl_monitor_vty_count--;
    return ;
}


int cl_vty_monitor_syslog_out( char * timestamp, int log_type, int log_level, char * logmsg, char * levelstr, char * typestr )
{
    unsigned int i;
    struct vty *vty;
    //char monitor_time_str[ 32 ];

    if ( cl_monitor_vty_count <= 0 )
    {
        return 1;
    }


    for ( i = 0; i < cl_vector_max( cl_vtyvec_master ); i++ )
    {
        if ( ( vty = cl_vector_slot( cl_vtyvec_master, i ) ) != NULL )
        {
            if ( ( vty->debug_monitor == 1 )
#ifdef _CL_CMD_PTY_RELAY_  
                && ( vty->cl_relay_fd == 0 )
#endif
            )
            {
                #if 0 // syslog
                if ( log_level == LOG_DEBUG_OUT ) /*debug out 直接输出*/
                {
                    if ( log_type == DEBUG_TCP_CONSOLE ) /*TCP 只向console输出*/
                    {
                        if ( vty->conn_type != _CL_VTY_CONSOLE_ )
                        {
                            continue ;
                        }
                    }
                    VOS_MemZero( monitor_time_str, 32 );
                    
                    switch ( vty->monitor_conf.cTime_Stamp )
                    {
                        case 1:
                            {
                                VOS_MemCpy( monitor_time_str, timestamp + 7, 8 );
                            }
                            break;

                        case 2:
                            {
                                VOS_MemCpy( monitor_time_str, timestamp, 15 );
                            }
                            break;

                        default:
                            break;
                    }
                    
                    VOS_StrCat( monitor_time_str, " " );
                    vty_direct_out( vty, "\r\n   %% %s%-8s%-8s %s", monitor_time_str, levelstr, typestr, logmsg );
                }
                else if ( ( vty->monitor_conf.cMonitor_Type_State[ log_type ] == 1 ) /*syslog */
                          &&
                          ( vty->monitor_conf.cMonitor_Level >= log_level )
                        )
                {
                    VOS_MemZero( monitor_time_str, 32 );
                    switch ( vty->monitor_conf.cTime_Stamp )
                    {
                        case 1:
                            {
                                VOS_MemCpy( monitor_time_str, timestamp + 7, 8 );
                            }
                            break;

                        case 2:
                            {
                                VOS_MemCpy( monitor_time_str, timestamp, 15 );
                            }
                            break;

                        default:
                            break;
                    }
                    VOS_StrCat( monitor_time_str, " " );
                    vty_direct_out( vty, "\r\n   %% %s%-8s%-8s %s", monitor_time_str, levelstr, typestr, logmsg );
                }
                #endif
            }
        }
    }

    return 1;
}


int cl_vty_freeze( void * arg, int frozen_type )
{
    struct vty * vty;

    vty = ( struct vty * ) arg;
    if ( vty == NULL )
    {
        return 0;
    }
    if ( vty->frozen == 0 )
    {
        vty->frozen = ( unsigned short ) frozen_type;
        if ( frozen_type == 2 )
        {
            // TODO ???
        }
    }

    return 1;
}

int cl_vty_unfreeze( void * arg, int unfrozen_to )
{
    struct vty * vty;
    int or_frozen;
    vty = ( struct vty * ) arg;
    if ( vty == NULL )
    {
        return 0;
    }
    if ( vty->frozen == 0 )
    {
        return 0;
    }
    or_frozen = vty->frozen;
    vty->frozen = ( unsigned short ) unfrozen_to;

    vty_buffer_reset( vty ) ;

    vty_out( vty, "%s", VTY_NEWLINE );
    vty_prompt ( vty );
    vty_out( vty, "%s", VTY_NEWLINE );	/* 防止提示符在同一行打印2次 */
    vty_flush_all( vty );

    if ( or_frozen == 2 )
    {
        vty_event( VTY_READ, vty->fd, vty );
    }

    return 1;
}

int cl_vty_unfreeze_and_put( void * arg, int unfrozen_to )
{
	struct vty * vty;
	int or_frozen;
	vty = ( struct vty * ) arg;
	if ( vty == NULL )
	{
		return 0;
	}
	if ( vty->frozen == 0 )
	{
		return 0;
	}
	or_frozen = vty->frozen;
	vty->frozen = ( unsigned short ) unfrozen_to;

	vty_buffer_reset( vty ) ;

	vty_out( vty, "%s", VTY_NEWLINE );
	vty_prompt ( vty );
	vty_flush_all( vty );

	if ( or_frozen == 2 )
	{
		vty_event( VTY_READ, vty->fd, vty );
	}
	vty_put( vty );
	return 1;
}
#if 0

/* Small utility function which output loggin to the VTY. */
void vty_log ( const char * proto_str, const char * format, va_list va )
{
    unsigned int i;
    struct vty *vty;

    for ( i = 0; i < cl_vector_max ( cl_vtyvec_master ); i++ )
    {
        if ( ( vty = cl_vector_slot ( cl_vtyvec_master, i ) ) != NULL )
        {
            if ( vty->monitor )
            {
                vty_out ( vty, "%s: ", proto_str );
                /*
                vvty_out (vty, format, va);
                */
                vty_out ( vty, "\r\n" );
                vty_event ( VTY_WRITE, vty->fd, vty );
            }
        }
    }
}
#endif

int cl_exec_timeout_show( struct vty * vty )
{
    unsigned long min;

    if ( vty->v_timeout )
    {
        min = ( vty->v_timeout / 60 );
        vty_out( vty, "  Idle time out is set to %d minutes.%s", min, VTY_NEWLINE );
    }
    else
    {
        vty_out( vty, "  Idle time out is set to disable.%s", VTY_NEWLINE );
    }

    return CMD_SUCCESS;
}

/* Set time out value. */
int cl_exec_timeout ( struct vty * vty, char * min_str, char * sec_str )
{
    unsigned long timeout = 0;

    /* min_str and sec_str are already checked by parser.  So it must be
    all digit string. */
    if ( min_str )
    {
        timeout = VOS_StrToUL ( min_str, NULL, 10 );
        timeout *= 60;
    }
    if ( sec_str )
    {
        timeout += VOS_StrToUL( sec_str, NULL, 10 );
    }
    if( (_CL_VTY_SSH_ == vty->conn_type) &&
        (0 == timeout) )
    {        
        vty_out(vty, "%% Error, The ssh's idle timeout must be limited.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    vty->v_timeout = timeout;
    vty_event ( VTY_TIMEOUT_RESET, vty->fd, vty );

    return CMD_SUCCESS;
}


/** 
 * 配置/获取 vty超时时间
 * @param[in ]   vty         vty
 * @param[in ]   newTime     新的超时时间，大于等于0时有效，0表示不会超时
 * @param[out]   oldTime     如果不为NULL 返回配置前的超时时间
 * @return      成功返回 VOS_OK，失败则返回其他
 */ 
LONG vty_timeout_config ( struct vty * vty, LONG newTime, LONG *oldTime)
{
    if( (_CL_VTY_SSH_ == vty->conn_type) &&
        (0 == newTime) )
    {        
        vty_out(vty, "%% Error, The ssh's idle timeout must be limited.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if(oldTime)
    {
        *oldTime = vty->v_timeout;
    }

    if(newTime >= 0)
    {
        vty->v_timeout = newTime;
        vty_event ( VTY_TIMEOUT_RESET, vty->fd, vty );
    }

    return CMD_SUCCESS;
}

/* When time out occur output message then close connection. */
int vty_timeout ( struct cl_lib_thread * thread )
{

    struct vty *vty;
    
    vty = cl_lib_THREAD_ARG ( thread );

#ifdef _CL_CMD_PTY_RELAY_  
    if(vty->cl_vty_in_relay)
    {
        vty_event(VTY_TIMEOUT_RESET, vty->fd, vty);
        return 0;
    }
#endif

    vty->t_timeout = NULL;
    vty->v_timeout = 0;

    vty_buffer_reset( vty ) ;
    vty_out ( vty, "\r\n\r\n  %% Connection idle time out.%s", VTY_NEWLINE );
    /*vty_flush_all(vty) ;*/
#if 0
    /* Close connection. */
    for(loop=0;loop<VTY_TIMEOUT_EVENT_MAX;loop++)
    {
        func = GetVtyTimeoutHandlerFunc(loop);
        if(func)
            (*(func))(vty);
    }
#endif    
    vty->status = VTY_CLOSE;
    vty_task_exit ( vty );

    return 0;
}


int vty_config_lock ( struct vty * vty )
{
	int i ;
	struct vty *v ;

    if (vty->conn_type == _CL_VTY_CONSOLE_ )
    {
          if ( cl_config_mode_lock )
            {
              /*kick off telnet user*/
              for ( i = 0; i < cl_vector_max( cl_vtyvec_master ); i++ )
                {
                  v = cl_vector_slot( cl_vtyvec_master, i );
                  if ( v == NULL )
                    continue;
                  if ( v->config == 0 )
                    continue;
                  if ( v->conn_type == _CL_VTY_CONSOLE_ )
                    continue;
                  vty_out( v, "\r\n  Console user %s is entering configuration mode\r\n", vty->user_name );
                  v->config = 0;

                  cl_vty_close_telnet_admin_by_console( vty, v->fd );
                   /*   break;*/
                }
            }
          cl_config_mode_lock = 1;

        vty->config = 1;
        return 1;
    }
    if ( cl_config_mode_lock == 0 )
    {
        vty->config = 1;
        cl_config_mode_lock = 1;
    }

    return vty->config;
}

int vty_config_unlock ( struct vty * vty )
{
    if ( vty->conn_type == _CL_VTY_CONSOLE_ )
    {
        vty->config = 0;
        if(cl_config_session_count)
            cl_config_session_count--;
        if(cl_config_session_count == 0)
          cl_config_mode_lock = 0;

        return 0;
    }
    if ( cl_config_mode_lock == 1 && vty->config == 1 )
    {
        vty->config = 0;
        if(cl_config_session_count)
            cl_config_session_count--;
        if(cl_config_session_count == 0)
        cl_config_mode_lock = 0;
    }

    return vty->config;
}


/** 
 * 控制台接入处理函数
 * @param[in]   thread     任务
 * @return      成功返回 VOS_OK,失败返回其他
 */
static int cl_vty_accept_console ( struct cl_lib_thread * thread )
{
    int vty_tty_fd;
    union cl_lib_sockunion su;
    unsigned char buf[ VTY_READ_BUFSIZ ];
    int readsize;
    int i;

    vty_tty_fd = cl_lib_THREAD_FD ( thread );
		
//VOS_Printf("\r\n #########SSSSSSSSS########%s.%d\r\n",__FUNCTION__,__LINE__);
    readsize = cl_read ( vty_tty_fd, ( char * ) buf, VTY_READ_BUFSIZ );

    if ( readsize > 0 )
    {

        /**********************************************************/
        /*
        if we want it active when press any key then use this 
        linux_memset (&su, 0, sizeof (union cl_lib_sockunion));
        su.sa.sa_family = AF_TTYS;
        vty = vty_create (vty_tty_fd,&su);
        return 0;
        */
        /*****************************************************/

        /* if we want it active only when return pressed then use this */
        for ( i = 0; i < readsize; i++ )
        {
            switch ( buf[ i ] )
            {
                case '\n':
                case '\r':
                    {
                        VOS_MemZero ( &su, sizeof ( union cl_lib_sockunion ) );
                        su.sa.sa_family = AF_TTYS;
                        vty_create ( vty_tty_fd, &su );

                        return VOS_OK;
                    }
                    break;
#ifdef _DEBUG_VERSION_UNUSE_

                case CONTROL ( 'R' ) :
                    {
                        reset();
                    }
                    break;
#endif

                default:
                    break;
            }
        }
    }

    cl_vty_serv_console ( vty_tty_fd );

    return VOS_OK;
}


/******************************************************
   socks and console service related function
 
 Serial Console connection     part
*****************************************************/

/** 
 * 添加控制台到接入源
 * @param[in]   console_fd     控制台标准输入fd
 * @return      成功返回 VOS_OK,失败返回其他
 */
int cl_vty_serv_console( int console_fd )
{
    struct cl_lib_thread * vty_serv_thread;

    /***** say hello to the console user *************/

#if (OS_LINUX)

    {
        cl_write( console_fd, _CL_TTY_PROMPT_, VOS_StrLen( _CL_TTY_PROMPT_ ) );
        cl_write( console_fd, "\r\nPress Enter to connect and config this system. ", VOS_StrLen("Press Enter to connect and config this system. "));
        cl_write( console_fd, "\r\n", VOS_StrLen("\r\n") );
    }

#endif /* (OS_LINUX) */


    vty_serv_thread =
        cl_lib_thread_add_read ( cl_thread_master, cl_vty_accept_console, NULL, console_fd );
    cl_vector_set_index ( cl_vty_serv_thread, console_fd, vty_serv_thread );

#ifdef _CL_RELAY_TELNET_OPTION_SERV_DEBUG_

    console_debug_vty = vty_new ();
    if ( console_debug_vty == NULL )
        return VOS_ERROR;
    console_debug_vty->type = VTY_TERM;
    console_debug_vty->fd = cl_serv_console_fd;
    console_debug_vty->conn_type = _CL_VTY_CONSOLE_;
    console_debug_vty->status = VTY_NORMAL;
#endif

    return VOS_OK;
}

#ifdef _MN_HAVE_TELNET_
ULONG                   semTelnet;
ULONG                   semTelnetV6;
#define VOS_ClI_DEFAULT_TELNET_PORT   9898
LONG cli_telnet_port = VOS_ClI_DEFAULT_TELNET_PORT;



/** 
 * telnet接入处理函数
 * @param[in]   thread     任务
 * @return      成功返回 VOS_OK,失败返回其他
 */
static int cl_vty_accept_telnet ( struct cl_lib_thread * thread )
{
    int vty_sock;
    union cl_lib_sockunion su;
    int ret;
    unsigned int on;
    int accept_sock;


    accept_sock = cl_lib_THREAD_FD ( thread );

    /* We continue hearing vty socket. */
    cl_vty_serv_telnet(accept_sock);

    VOS_MemZero ( &su, sizeof ( union cl_lib_sockunion ) );

    /* We can handle IPv4 or IPv6 socket. */
    vty_sock = cl_lib_sockunion_accept ( accept_sock, &su );
    if ( vty_sock < 0 )
    {
        /*
        VOS_Printf ("can't accept vty socket : %s", strerror (errno));
        */
        VOS_ASSERT( 0 );
        return -1;
    }

    {
        char addr[128] = {0};
        int noLogin = 0;

        VOS_MemSet(addr,0,128);
        cl_lib_inet_sutop(&su,addr);

        if((VOS_FALSE == foreign_login_enable) 
            && (VOS_FALSE == cl_lib_sockunion_is_localAddr(&su)))
        {
            noLogin = 1;
        }

        if(noLogin)
        {
            VOS_Printf("client %s is forbidden to login\r\n",addr);
            close(vty_sock);
            return -1;
        }
    }


    on = 1;
    ret = setsockopt ( vty_sock, IPPROTO_TCP, TCP_NODELAY,
                       ( char * ) &on, sizeof ( on ) );
    if ( ret < 0 )
    {
        /*
        VOS_Printf("can's set sock opt.\r\n");
        */
        VOS_ASSERT( 0 );
        return ret;
    }

    vty_create ( vty_sock, &su );

    return 0;
}


/** 
 * 添加telnet到接入源
 * @param[in]   sock     控制台标准输入fd
 * @return      成功返回 VOS_OK,失败返回其他
 */
static int cl_vty_serv_telnet( int sock )
{
    struct cl_lib_thread *vty_serv_thread;
    union cl_lib_sockunion *su = cl_lib_sockunion_getsockname (sock);
    vty_serv_thread = cl_lib_thread_add_read ( cl_thread_master , cl_vty_accept_telnet, NULL, sock );
#ifdef _MN_HAVE_TELNET_V6_
	if(su && su->sa.sa_family == AF_INET6)
	{
        VOS_Printf("telnet IPV6 init\r\n");
        cl_vector_set_index ( cl_vty_serv_thread, sock, vty_serv_thread );
        cl_serv_v6_telnet_fd = sock;
	}
	else
#endif	
	{
        VOS_Printf("telnet IPV4 init\r\n");
        cl_vector_set_index ( cl_vty_serv_thread, sock, vty_serv_thread );
        cl_serv_telnet_fd = sock;
    }

    if(su)
    {
        VOS_Free(su);
    }

    return VOS_OK;

}

/****************************************
telnet related . 
*************************************/
int cl_vty_serv_sock_family ( unsigned short port, int family )
{
    int ret;
    union cl_lib_sockunion su;
    int accept_sock;

    VOS_MemZero ( &su, sizeof ( union cl_lib_sockunion ) );
    su.sa.sa_family = ( unsigned short ) family;

    /* Make new socket. */
    accept_sock = cl_lib_sockunion_stream_socket ( &su );
    if ( accept_sock < 0 )
    {
        VOS_ASSERT( 0 );
        return -1;
    }

    /* This is server, so reuse address. */
    ret = cl_lib_sockopt_reuseaddr ( accept_sock );
    if ( ret < 0 )
    {
        VOS_ASSERT( 0 );
        return -1;
    }
    ret = cl_lib_sockopt_reuseport ( accept_sock );
    if ( ret < 0 )
    {
        VOS_ASSERT( 0 );
        return -1;
    }


    /* Bind socket to universal address and given port. */
    ret = cl_lib_sockunion_bind ( accept_sock, &su, port, NULL );
    if ( ret < 0 )
    {
        VOS_ASSERT( 0 );
        return -1;
    }

    /* Listen socket under queue 3. */
    ret = listen ( accept_sock, 3 );

    if ( ret < 0 )
    {
        /*
        syslog (NULL, LOG_WARNING, "cannot listen socket...");
        */
        VOS_ASSERT( 0 );
        return -1;
    }

    /* Add vty server. */
    cl_vty_serv_telnet(accept_sock);

    return accept_sock;
}

void cl_vty_serv_telnet_enable( unsigned short port )
{
    cl_serv_telnet_fd = cl_vty_serv_sock_family ( port, AF_INET );
#ifdef _MN_HAVE_TELNET_V6_	
    cl_serv_v6_telnet_fd = cl_vty_serv_sock_family ( port, AF_INET6 );
#endif
    return ;
}

void cl_vty_serv_telnet_disable( void )
{
    struct cl_lib_thread * serv_thread;

    serv_thread = cl_vector_slot( cl_vty_serv_thread, cl_serv_telnet_fd );

    cl_vector_unset( cl_vty_serv_thread, cl_serv_telnet_fd );

    {
        cl_lib_thread_cancel( serv_thread );
    }
#ifdef _MN_HAVE_TELNET_V6_	
   serv_thread = cl_vector_slot( cl_vty_serv_thread, cl_serv_v6_telnet_fd );

    cl_vector_unset( cl_vty_serv_thread, cl_serv_v6_telnet_fd );

    cl_lib_thread_cancel( serv_thread );
#endif

    /* telnet服务的disable的时候,telnet的主任务的监听描述符仍然在select,必须等待
    select返回之后才能关闭telnet的socket的描述符,这里用信号量作的互斥by liuyanjun 2002/08/28 */
#ifdef _MN_HAVE_TELNET_
    VOS_SemTake( semTelnet, VOS_WAIT_FOREVER );
#endif /* #ifdef _MN_HAVE_TELNET_ */

    cl_close( cl_serv_telnet_fd );
    cl_serv_telnet_fd = -1;

#ifdef _MN_HAVE_TELNET_

    VOS_SemGive( semTelnet );
#endif /* #ifdef _MN_HAVE_TELNET_ */

#ifdef _MN_HAVE_TELNET_V6_
    VOS_SemTake( semTelnetV6, VOS_WAIT_FOREVER );


    cl_close( cl_serv_v6_telnet_fd );
    cl_serv_v6_telnet_fd = -1;

    VOS_SemGive( semTelnetV6);
#endif /* #ifdef _MN_HAVE_TELNET_V6_ */
    return ;
}


void vos_telnet_init()
{
    semTelnet = VOS_SemBCreate(VOS_SEM_Q_PRIORITY, VOS_SEM_FULL);
#ifdef _MN_HAVE_TELNET_V6_
    semTelnetV6= VOS_SemBCreate(VOS_SEM_Q_PRIORITY, VOS_SEM_FULL);
#endif

    cl_vty_serv_telnet_enable(cli_telnet_port);
}
#endif


/***********************************************************
 
vty utility lib that can be called by command 
 
*****************************************************/
int cl_vty_close_all_vty( void )
{
    unsigned int i;
    struct vty *vty;

    for ( i = 0; i < cl_vector_max( cl_vtyvec_master ); i++ )
    {
        if ( ( vty = cl_vector_slot( cl_vtyvec_master, i ) ) != NULL )
        {
            vty_buffer_reset( vty ) ;
            vty_out( vty, "%s  System is going to reboot......%s", VTY_NEWLINE, VTY_NEWLINE );
            vty->status = VTY_CLOSE;
            vty_reboot( vty );
        }
    }

    return 1;
}

int cl_vty_exit_all_vty(void)
{
	unsigned int i;
	struct vty *vty;
	
	for ( i = 0; i < cl_vector_max( cl_vtyvec_master ); i++ )
	{
		if ( ( vty = cl_vector_slot( cl_vtyvec_master, i ) ) != NULL )
		{
			vty_buffer_reset( vty ) ;
			vty->status = VTY_CLOSE;
			vty_task_exit(vty);
		}
	}
	return 1;
     
}
int cl_vty_close_console_vty(void)
{
    unsigned int i;
    struct vty *vty;
    for ( i = 0; i < cl_vector_max( cl_vtyvec_master ); i++ )
    {
        if ( ( vty = cl_vector_slot( cl_vtyvec_master, i ) ) != NULL )
        {
            if ( vty->conn_type == _CL_VTY_CONSOLE_)
            {
                vty_out( vty, "%s  Quit console session, please relogin later.%s", VTY_NEWLINE, VTY_NEWLINE );
                vty->status = VTY_CLOSE;
                VOS_TaskTerminate( ( VOS_HANDLE ) vty->sub_task_id, 1 );
                vty->cl_vty_exit = 1 ;
                vty->sub_thread_master->exit_flag = 1;
            }
        }
    }

    return 0;
}


int cl_vty_close_all_telnet_vty( struct vty * v )
{
    unsigned int i;
    struct vty *vty;
    for ( i = 0; i < cl_vector_max( cl_vtyvec_master ); i++ )
    {
        if ( ( vty = cl_vector_slot( cl_vtyvec_master, i ) ) != NULL )
        {
            if (( vty->conn_type == _CL_VTY_TELNET_ )||(vty->conn_type == _CL_VTY_TELNET_V6_))
            {
                vty_out( vty, "  %sTelnet server down.%s", VTY_NEWLINE, VTY_NEWLINE );
                vty->status = VTY_CLOSE;
                VOS_TaskTerminate( ( VOS_HANDLE )vty->sub_task_id, 1 );
                vty->cl_vty_exit = 1 ;
                vty->sub_thread_master->exit_flag = 1;
            }
        }
    }

    return 0;
}

int cl_vty_close_by_username( char * del_username, struct vty * v )
{
    unsigned int i;
    struct vty *vty;

    for ( i = 0; i < cl_vector_max( cl_vtyvec_master ); i++ )
    {
        if ( ( vty = cl_vector_slot( cl_vtyvec_master, i ) ) != NULL )
        {
            if ( VOS_StrCmp( vty->user_name, del_username ) == 0 )
            {
                vty_out( vty, "%s  %% Your user name has been deleted from system.%s", VTY_NEWLINE, VTY_NEWLINE );
                vty->status = VTY_CLOSE;
                if ( v != vty )
                {
                    if ( VOS_TaskHandleVerify( vty->sub_task_id ) )
                    {
                        /* 先向任务发消息,使协议栈的阻塞函数退出*/
                        if ( vty->conn_type != _CL_VTY_CONSOLE_ )
                        {
                            VOS_TaskTerminate( ( VOS_HANDLE )vty->sub_task_id, 1 );
                        }
                        vty->cl_vty_exit = 1 ;
                        vty->sub_thread_master->exit_flag = 1;
                    }
                    else
                    {
                        VOS_ASSERT( 0 );
                    }
                }
            }
        }
    }

    return 0;
}

int cl_vty_close_by_sessionid( struct vty * v, int id )
{
    unsigned int i;
    struct vty *vty;

    for ( i = 0; i < cl_vector_max( cl_vtyvec_master ); i++ )
    {
        if ( ( vty = cl_vector_slot( cl_vtyvec_master, i ) ) != NULL )
        {
            if ( vty->fd == id )
            {
                if ( vty->conn_type == _CL_VTY_CONSOLE_ && cl_vty_user_limit == 1 )
                {
                    vty_out( v, "%% Error, You can't close a console session.\r\n" );
                    return 0;
                }
                vty_buffer_reset( vty ) ;
                vty_out( vty, "%s  %% The connection is closed by %s.%s", VTY_NEWLINE, v->user_name, VTY_NEWLINE );
                /*vty_flush_all( vty) ;*/
                vty->status = VTY_CLOSE;
                if ( v != vty )
                {
                    if ( VOS_TaskHandleVerify( vty->sub_task_id ) )
                    {
                        /* 先向任务发消息,使协议栈的阻塞函数退出*/
			   if ( vty->conn_type != _CL_VTY_CONSOLE_ )
                        {
                            VOS_TaskTerminate( ( VOS_HANDLE )vty->sub_task_id, 1 );
                        }
                        vty->cl_vty_exit = 1 ;
                        vty->sub_thread_master->exit_flag = 1;
                    }
                    else
                    {
                        VOS_ASSERT( 0 );
                    }

                    /* Unset cl_vector. */
                    cl_vector_unset ( cl_vtyvec_master, vty->fd );
                    cl_vty_session_count -- ;
                }

                return 1;
            }
        }
    }

    vty_out( v, "  %% No such session id [%d].\r\n", id );
    return 0;
}

int cl_vty_close_telnet_admin_by_console(struct vty *v, int id)
{
	unsigned int i;
	struct vty *vty;
  
	for (i = 0; i < cl_vector_max(cl_vtyvec_master); i++)
	{
		if ((vty = cl_vector_slot(cl_vtyvec_master,i)) != NULL)
		{
			if (vty->fd == id)
			{
				if (vty->conn_type == _CL_VTY_CONSOLE_ && cl_vty_user_limit == 1 )
				{
					vty_out(v,"%% Error, You can't close a console session.\r\n");
					return 0;
				}
				vty_out(vty,"  %sExit config mode because another user has entered config mode via console.%s",VTY_NEWLINE,VTY_NEWLINE);

                vty->node = VOS_CLI_VIEW_NODE;
                /* close telnet PTY */
                #ifdef _CL_CMD_PTY_RELAY_  
                if((vty->cl_vty_in_relay  == 1) && (vty->cl_vty_relay_type == 2))
                {
                    vty->cl_vty_relay_exit = 1;
                }
				else if((vty->cl_vty_in_relay  == 1) && (vty->cl_vty_relay_type == 3))
                {
                    vty->cl_vty_relay_exit = 1;
                }
                #endif
                if(cl_config_session_count)
                    cl_config_session_count--;
				vty_prompt(vty);
				vty_event(VTY_WRITE, vty->fd, vty);
			       return 1;
			}		 
		}	 
	}
	
	vty_out(v,"  No such session id [%d].\r\n",id);
	return 0;
}

void initVtyTimeoutCallbackFunctions()
{
    VOS_MemSet(g_vty_timeout_callbacks, 0, sizeof(g_vty_timeout_callbacks));
}
vty_timeout_callback_func GetVtyTimeoutHandlerFunc(g_vty_timeout_code_t code)
{
    if(code>vty_timeout_code_min &&code<vty_timeout_code_max)
    {
        if(g_vty_timeout_callbacks[code])
        {
            return g_vty_timeout_callbacks[code];
        }
    }
    return NULL;
}
int registerVtyTimeoutCallback(g_vty_timeout_code_t code, vty_timeout_callback_func  pfunc)
{
    int ret = VOS_ERROR;

    if(code>vty_timeout_code_min &&code<vty_timeout_code_max)
    {
        if(!g_vty_timeout_callbacks[code])
        {
            g_vty_timeout_callbacks[code]= pfunc;
            ret = VOS_OK;
        }
    }

    return ret;
}

int unregisterVtyTimeoutCallback(g_vty_timeout_code_t code)
{
    int ret = VOS_ERROR;

    if(code>vty_timeout_code_min &&code<vty_timeout_code_max)
    {
        g_vty_timeout_callbacks[code] = 0;
        ret = VOS_OK;
    }

    return ret;
}
extern int OnuProfile_VtytimeoutCallback(struct vty *vty);

void init_vty_timeout_install()
{
    //registerVtyTimeoutCallback(vty_timeout_code_profile, OnuProfile_VtytimeoutCallback);
}


/**************************************************
 
 init vty struct  and cl_vty_serv_thread
 
 and init some global val
 
*******************************************************/

/** 
 * 创建会话根数组
 * @return      成功返回 VOS_OK,失败返回其他
 */
void cl_vty_init ()
{
    /* For further configuration read, preserve current directory. */
    cl_vtyvec_master = cl_vector_init ( _CL_VECTOR_MIN_SIZE_ );
    if ( cl_vtyvec_master == NULL )
        return ;
    /* Initilize server thread cl_vector. */
    cl_vty_serv_thread = cl_vector_init ( _CL_VECTOR_MIN_SIZE_ );
    if ( cl_vty_serv_thread == NULL )
        return ;
    cl_vty_session_count = 0;
    cl_config_mode_lock = 0;
    cl_monitor_vty_count = 0;
    cl_config_session_count = 0;
    initVtyTimeoutCallbackFunctions();
    init_vty_timeout_install();    
    return ;
}

/*** 产品判断vty是否依然存在*******/
int cl_vty_valid( struct vty * v )
{
    unsigned int i;
    struct vty *vty;
    int found=0;
    
    for ( i = 0; i < cl_vector_max( cl_vtyvec_master ); i++ )
    {
        if ( ( vty = cl_vector_slot( cl_vtyvec_master, i ) ) != NULL )
        {           
            if ( vty == v )
            {
                found = 1;
                break;
            }
        }
    }
    if( found == 1 )
    	return 0;
    else
    	return -1;
}


#ifdef __cplusplus
}
#endif



