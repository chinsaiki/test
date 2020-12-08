#include <fcntl.h>

#include "config.h"
#include "cli.h"
#include "vos_vty.h"


LONG foreign_login_enable = 1;

int cl_serv_telnet_fd;
int cl_serv_v6_telnet_fd;
int cl_serv_console_fd;

cl_vector cl_cmdvec_master;
cl_vector cl_vtyvec_master = NULL;

static cl_vector cl_vty_serv_thread;

/* Configure lock. */
static int cl_config_mode_lock;
int cl_config_session_count;
int cl_vty_session_count ;

/* monitor vty_count */
static int cl_monitor_vty_count;


#define VTY_TIMEOUT_EVENT_MAX 10
typedef int (*vty_timeout_callback_func)(struct vty *vty);  
vty_timeout_callback_func g_vty_timeout_callbacks[VTY_TIMEOUT_EVENT_MAX];


/* 初始化向量，size指向量表的大小--可以容纳多少个向量。 */
/* Initialize cl_vector : allocate memory and return cl_vector. */
cl_vector cl_vector_init( unsigned int size )
{
    cl_vector v = ( cl_vector ) VOS_Malloc( sizeof ( struct _cl_vector ), 0 );
    if ( v == NULL )
        return NULL;
    /* allocate at least one slot */
    if ( size == 0 )
    {
        size = 1;
    }

    v->alloced = size;
    v->max = 0;
    v->index = ( void * ) VOS_Malloc( sizeof ( void * ) * size, 0 );
    if ( v->index == NULL )
    {
        VOS_Free( v );
        return NULL;
    }
    VOS_MemSet( v->index, 0, sizeof ( void * ) * size );

    return v;
}


/** 
 * 创建命令根数组
 * @return      成功返回 VOS_OK,失败返回其他
 */
int cl_cmd_init ( void )
{
    /*
     * Allocate initial top cl_vector of commands.
     */
    cl_cmdvec_master = cl_vector_init ( _CL_VECTOR_MIN_SIZE_ );
    if ( NULL == cl_cmdvec_master )
    {
        VOS_ASSERT( 0 );  /*There is serious problems.*/
        return VOS_ERROR;
    }
    return VOS_OK;
}

void initVtyTimeoutCallbackFunctions()
{
    VOS_MemSet(g_vty_timeout_callbacks, 0, sizeof(g_vty_timeout_callbacks));
}
void init_vty_timeout_install()
{
    //registerVtyTimeoutCallback(vty_timeout_code_profile, OnuProfile_VtytimeoutCallback);
}


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


int cli_cmd_parser_init(void)
{
    /* build command vector tree */
    cl_cmd_init ();

    /* build vty vector tree */
    cl_vty_init ();

    /* build command module link */
    //cl_modules_init ();

    /********************************
    install basic command node and command element 
    ********************************/
    cl_install_basic_command();

    
    return 0;
}


/** 
 * 命令行模块初始化函数
 * @param[in]   console_enable     是否监控linux控制台，前台运行app时才起作用
 * @return      成功返回 VOS_OK,失败返回其他
 */
LONG vos_cli_init( LONG  console_enable)
{
    LONG ret = VOS_ERROR;
    
    cl_serv_telnet_fd = -1;
    cl_serv_v6_telnet_fd = -1;	
    cl_serv_console_fd = -1;


//    VOS_StrCpy(cli_module.name,MOD_NAME_VOS_CLI);


//    ret= VOS_module_register(cli_module.name,&cli_module);
//    if(VOS_OK != ret)
//    {
//        VOS_ASSERT(0);
//        cli_err_print("cli mod reg err %d",ret);
//    }

//    g_cli_lock = VOS_SemMCreate(VOS_SEM_Q_FIFO);
//    if(!g_cli_lock)
//    {
//        vos_err_print("create cli lock error\r\n");
//        VOS_ASSERT(0);
//        return VOS_ERROR;
//    }


//    vos_config_parse_cli();

    cli_cmd_parser_init();
    cl_main_start();

    vos_telnet_init();

    if(console_enable)
    {
        cl_serv_console_fd = open("/dev/stdout", O_RDWR |O_NONBLOCK  ); /* FOR PC USE */;
        VOS_Console_FD = cl_serv_console_fd;
        linux_Console_Init(cl_serv_console_fd);
        cl_vty_serv_console( cl_serv_console_fd );
    }

    return VOS_OK;
}

