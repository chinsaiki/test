
/**
 * @file cl_main.c
 * @brief cli 
 * @details 命令行主任务
 * @version 1.0
 * @author  wujianming  (wujianming@sylincom.com)
 * @date 2019.01.25
 * @copyright 
 *    Copyright (c) 2018 Sylincom.
 *    All rights reserved.
*/


#ifdef __cplusplus
extern"C"
{
#endif

#include "vos_global.h"
#include "vos_common.h"
#include "vos/vos_conf/vos_conf_prv.h"
#include "platform_module_names.h"

#include "cli/cli.h"
#include "cli/cl_cmd.h"
#include "cli/cl_vty.h"
#include "cli/cl_bcmd.h"
#include "cli/cl_set.h"

#include "lib/cl_lib_thread.h"

#if OS_LINUX 
#include <fcntl.h>
#include <termios.h>
#endif /* OS_LINUX */
#ifdef PLAT_MODULE_SYSLOG
#include <plat_syslog.h>
#endif

VOS_HANDLE hCl_Task=NULL;


LONG foreign_login_enable = _CL_FOREIGN_LOGIN_ENABLE;
int cl_serv_telnet_fd;
int cl_serv_v6_telnet_fd;
int cl_serv_console_fd;
vos_module_t cli_module;
LONG g_cli_lock;


extern INT VOS_Console_FD;
extern void vos_cmd_install(void);
/* Master of threads. */
struct cl_lib_thread_master *cl_thread_master;


/* 在主任务中添加对命令行多任务的监视,包括串口的子任务和其他的telnet的子任务
   有子任务死掉或者没有响应,就杀掉这个子任务,如果是串口就重新启动串口的描述符
   用于再次登陆 by liuyanjun 2002/07/06 */
cliTaskActive consoleTaskActive;
cliTaskActive telnetTaskActive[ CL_TELNET_MAX ];



/* 在主任务中添加对命令行多任务的监视,包括串口的子任务和其他的telnet的子任务
   有子任务死掉或者没有响应,就杀掉这个子任务,如果是串口就重新启动串口的描述符
   用于再次登陆 by liuyanjun 2002/07/06 */

void initCliTaskStatus( cliTaskActive * taskActiveStatus )
{
    VOS_MemZero( ( char * ) taskActiveStatus, sizeof( cliTaskActive ) );
}

void initCliTaskActiveStatus()
{
    int i = 0;
    initCliTaskStatus( &consoleTaskActive );
    for ( i = 0;i < CL_TELNET_MAX; i++ )
    {
        initCliTaskStatus( &telnetTaskActive[ i ] );
    }
    return ;
}

void killCli( void * taskvty )
{
    /*    LRESULT lRet;*/
    vty_task_exit( ( struct vty * ) taskvty );
    /* because in vty_close function the console server is added, so don't add it here */
    /*    lRet = VOS_TaskTerminate(taskHandle, 0);*/
    /* because in vty_close function the console task is killed, so don't killed here*/
    return ;
}

int getTelnetActiveUnuse()
{
    int i;
    for ( i = 0;i < CL_TELNET_MAX; i++ )
    {
        if ( telnetTaskActive[ i ].userFlag == VOS_FALSE )
        {
            return i;
        }
    }
    return VOS_ERROR;
}

int getTelnetActiveTask( VOS_HANDLE telnetTaskId )
{
    int i;
    for ( i = 0;i < CL_TELNET_MAX; i++ )
    {
        if ( telnetTaskActive[ i ].taskHandle == telnetTaskId )
        {
            return i;
        }
    }
    return VOS_ERROR;

}

void cliSetTaskFlagWhenTaskCreate( void * taskvty )
{
    VOS_HANDLE cliTaskId = NULL;
    CHAR cliTaskName[ VOS_NAME_MAX_LENGTH ];
    LRESULT retTask;
    int retIndex = 0;

    cliTaskId = VOS_GetCurrentTask( );
    retTask = VOS_GetCurrentTaskName( cliTaskName );
    if ( retTask == VOS_ERROR )
    {}
    else
    {
        if ( VOS_StrCmp( cliTaskName, "tCliConsole" ) == 0 )      /* this is the console task*/
        {
            consoleTaskActive.userFlag = VOS_TRUE;
            consoleTaskActive.taskHandle = cliTaskId;
            consoleTaskActive.secondCount = 0;
            consoleTaskActive.taskVty = taskvty;
            VOS_StrCpy( consoleTaskActive.cliTaskName , cliTaskName );
            consoleTaskActive.active = VOS_TRUE;

        }
        else if ( VOS_MemCmp( cliTaskName, "tCli_", 5 ) == 0 )      /* this is the telnet task */
        {
            retIndex = getTelnetActiveUnuse();
            if ( retIndex == VOS_ERROR )
            {
                VOS_ASSERT( 0 );
                return ;  /* this case not occour */
            }
            initCliTaskStatus( &telnetTaskActive[ retIndex ] );

            telnetTaskActive[ retIndex ].userFlag = VOS_TRUE;
            telnetTaskActive[ retIndex ].taskHandle = cliTaskId;
            telnetTaskActive[ retIndex ].secondCount = 0;
            telnetTaskActive[ retIndex ].taskVty = taskvty;
            VOS_StrCpy( telnetTaskActive[ retIndex ].cliTaskName, cliTaskName );
            telnetTaskActive[ retIndex ].active = VOS_TRUE;

        }
        else if ( VOS_StrCmp( cliTaskName, "tClMain" ) == 0 )      /* this is command main task */
        {
            /* this is not occour*/
        }
        else                                        /* this is other task which use thread */
        {
            /* this is not occour*/
        }

    }

    return ;

}


void cliSetTaskNoActiveAfterSelect()
{
    VOS_HANDLE cliTaskId = NULL;
    CHAR cliTaskName[ VOS_NAME_MAX_LENGTH ];
    int retIndex;
    LRESULT retTask;

    cliTaskId = VOS_GetCurrentTask( );
    retTask = VOS_GetCurrentTaskName( cliTaskName );
    if ( retTask == VOS_ERROR )
    {}
    else
    {
        if ( VOS_StrCmp( cliTaskName, "tCliConsole" ) == 0 )      /* this is the console task*/
        {
            consoleTaskActive.userFlag = VOS_TRUE;
            consoleTaskActive.taskHandle = cliTaskId;
            consoleTaskActive.secondCount = 0;
            VOS_StrCpy( consoleTaskActive.cliTaskName , cliTaskName );
            consoleTaskActive.active = VOS_FALSE;

        }
        else if ( VOS_MemCmp( cliTaskName, "tCli_", 5 ) == 0 )      /* this is the telnet task */
        {
            retIndex = getTelnetActiveTask( cliTaskId );
            if ( retIndex == VOS_ERROR )
            {
                /* do nothing */
            }
            else
            {
                telnetTaskActive[ retIndex ].userFlag = VOS_TRUE;
                telnetTaskActive[ retIndex ].taskHandle = cliTaskId;
                telnetTaskActive[ retIndex ].secondCount = 0;
                VOS_StrCpy( telnetTaskActive[ retIndex ].cliTaskName , cliTaskName );
                telnetTaskActive[ retIndex ].active = VOS_FALSE;

            }
        }
        else if ( VOS_StrCmp( cliTaskName, "tClMain" ) == 0 )      /* this is command main task */
        {
            /* nothing to do */
        }
        else                                        /* this is other task which use thread */
        {
            /* nothing to do */
        }

    }

    return ;

}

void cliCheckSetActiveTaskAfterSelect()
{
    VOS_HANDLE cliTaskId = NULL;
    CHAR cliTaskName[ VOS_NAME_MAX_LENGTH ];
    int retIndex;
    int i;
    LRESULT retTask;


    cliTaskId = VOS_GetCurrentTask( );
    retTask = VOS_GetCurrentTaskName( cliTaskName );
    if ( retTask == VOS_ERROR )
    {}
    else
    {
        if ( VOS_StrCmp( cliTaskName, "tCliConsole" ) == 0 )      /* this is the console task*/
        {
            consoleTaskActive.userFlag = VOS_TRUE;
            consoleTaskActive.taskHandle = cliTaskId;
            consoleTaskActive.secondCount = 0;
            VOS_StrCpy( consoleTaskActive.cliTaskName , cliTaskName );
            consoleTaskActive.active = VOS_TRUE;

        }
        else if ( VOS_MemCmp( cliTaskName, "tCli_", 5 ) == 0 )      /* this is the telnet task */
        {
            retIndex = getTelnetActiveTask( cliTaskId );
            if ( retIndex == VOS_ERROR )
            {
                /* do nothing */
            }
            else
            {
                telnetTaskActive[ retIndex ].userFlag = VOS_TRUE;
                telnetTaskActive[ retIndex ].taskHandle = cliTaskId;
                telnetTaskActive[ retIndex ].secondCount = 0;
                VOS_StrCpy( telnetTaskActive[ retIndex ].cliTaskName , cliTaskName );
                telnetTaskActive[ retIndex ].active = VOS_TRUE;

            }

        }
        else if ( VOS_StrCmp( cliTaskName, "tClMain" ) == 0 )      /* this is command main task */
        {
            /* first check the console task */
            if ( consoleTaskActive.userFlag == VOS_TRUE )
            {
                if ( consoleTaskActive.active != VOS_TRUE )
                {
                    if ( consoleTaskActive.secondCount >= CL_TASK_DEAD_TIME )      /* beyond the dead time */
                    {
                        killCli( consoleTaskActive.taskVty );
                        initCliTaskStatus( &consoleTaskActive );
                        /* move to vty_close function */
                    }
                    else
                    {
                        consoleTaskActive.secondCount += 1;
                    }
                }
                else
                {
                    consoleTaskActive.secondCount = 0;

                }

            }

            /* second check the telnet task */
            for ( i = 0;i < CL_TELNET_MAX;i++ )
            {
                if ( telnetTaskActive[ i ].userFlag == VOS_TRUE )
                {
                    if ( telnetTaskActive[ i ].active != VOS_TRUE )
                    {
                        if ( telnetTaskActive[ i ].secondCount >= CL_TASK_DEAD_TIME )      /* beyond the dead time */
                        {
                            killCli( telnetTaskActive[ i ].taskVty );
                            initCliTaskStatus( &telnetTaskActive[ i ] );
                            /*initCliTaskStatus(&consoleTaskActive);*/
                            /* move to vty_close function */
                        }
                        else
                        {
                            telnetTaskActive[ i ].secondCount += 1;
                        }
                    }
                    else
                    {
                        telnetTaskActive[ i ].secondCount = 0;

                    }

                }

            }


        }
        else                                        /* this is other task which use thread */
        {
            /* do nothing */
        }

    }

    return ;

}

/* 在主任务中添加对命令行多任务的监视,包括串口的子任务和其他的telnet的子任务
   有子任务死掉或者没有响应,就杀掉这个子任务,如果是串口就重新启动串口的描述符
   用于再次登陆 by liuyanjun 2002/07/06 
   但此时忽略了串口和telnet的idletimeOut 情况,在idletimeout时这个命令行的子任务已经被删掉了
   所以在idletimeout时加入清除任务状态的函数调用*/

void cliClearActiveTaskWhenVtyClose()
{
    VOS_HANDLE cliTaskId = NULL;
    CHAR cliTaskName[ VOS_NAME_MAX_LENGTH ];
    int retIndex;
    LRESULT retTask;

    /*initCliTaskActiveStatus();*/

    cliTaskId = VOS_GetCurrentTask( );
    retTask = VOS_GetCurrentTaskName( cliTaskName );
    if ( retTask == VOS_ERROR )
    {
        VOS_ASSERT( 0 );
    }
    else
    {
        if ( VOS_StrCmp( cliTaskName, "tCliConsole" ) == 0 )      /* this is the console task*/
        {
            initCliTaskStatus( &consoleTaskActive );
        }
        else if ( VOS_MemCmp( cliTaskName, "tCli_", 5 ) == 0 )      /* this is the telnet task */
        {
            retIndex = getTelnetActiveTask( cliTaskId );
            if ( retIndex == VOS_ERROR )
            {
                /* do nothing */
            }
            else
            {
                initCliTaskStatus( &telnetTaskActive[ retIndex ] );
            }

        }
        else if ( VOS_StrCmp( cliTaskName, "tClMain" ) == 0 )      /* this is command main task */
        {
            /*            ASSERT(0);*/
        }
        else                                        /* this is other task which use thread */
        {
            /*            ASSERT(0);*/
        }

    }

    return ;
}

ULONG semDebugCli;

/** 命令行主任务，监听接入源列表 */
DECLARE_VOS_TASK( cl_Main )
{
    struct cl_lib_thread thread;

    VOS_TaskInfoAttach();

    /* 如果支持Console命令行。 */
#if 1 //( defined(VOS_HAVE_CLI) && defined(VOS_CLI_HAVE_CONSOLE) )
    {
        

        /* 在主任务中添加对命令行多任务的监视,包括串口的子任务和其他的telnet的子任务
           有子任务死掉或者没有响应,就杀掉这个子任务,如果是串口就重新启动串口的描述符
           用于再次登陆 */
        initCliTaskActiveStatus();

        /*---------------------------
        wake up the init for cli is ready and
        the start config is preformed
        -----------------------------------*/
        #ifdef PLAT_MODULE_SYSLOG
        VOS_log (MODULE_RPU_CLI,SYSLOG_INFO, "CLI started OK, begin to listen." );
        #endif

        /* Execute each thread. */
        while ( cl_lib_thread_fetch( cl_thread_master, &thread ) )
        {
            cl_lib_thread_call( &thread );
        }
        return ;
    }
#endif /* VOS_HAVE_CLI && VOS_CLI_HAVE_CONSOLE */

}


/** 
 * 启动命令行主任务
 * @return      成功返回 VOS_OK,失败返回其他
 */
int cl_main_start( void )
{
    ULONG ulArgs[ VOS_MAX_TASK_ARGS ];

    /* Prepare cl_thread_master thread. */
    cl_thread_master = cl_lib_thread_make_master ();

    VOS_Printf( "Done.    \r\n" );



    /* Sort all installed commands. */
    cl_sort_node ();



    ulArgs[ 0 ] = _CL_VTY_CONSOLE_;
    hCl_Task = VOS_TaskCreateEx( "tClMain", cl_Main, TASK_PRIORITY_HIGHER ,
                                 VOS_DEFAULT_STACKSIZE, ulArgs );

    return VOS_OK;
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

VOID cli_cmd_parser_final_init()
{
    cl_sort_node ();
}


VOID sys_stop_manage_service()
{
     extern int cl_vty_exit_all_vty(void);

     /*exit all vty user*/
     cl_vty_exit_all_vty();
     VOS_TaskDelay(10);  
}


/** 
 * 对linux控制台进行配置
 * @param[in]   console_fd     linux控制台标准输入fd
 */
void linux_Console_Init(int console_fd)
{
    struct termios tc;

    tcgetattr(console_fd, &tc);
    tc.c_lflag &= ~(ICANON | ECHO | ECHOE /*| ISIG */);
    tcsetattr(console_fd, TCSANOW, &tc);

    return;
}

#define CLI_CONF_FOREIGN_LOGIN_ENABLE_KEY "cli_foreign_login_enable"
#define CLI_CONF_TELNET_PORT_KEY "cli_telnet_port"
#define CLI_CONF_HOST_NAME_KEY "cli_host_name"
#define CLI_CONF_PRODUCT_NAME_KEY "cli_product_name"

extern LONG cli_telnet_port;
extern CHAR vos_hostname[];
extern CHAR vos_product_name[];

LONG vos_config_parse_cli(VOID)
{
    VOS_CONFIG_FILE *vos_conf = VOS_config_vos_file_open();

    if(vos_conf)
    {
        LONG size = 0;
        VOS_config_get_number(vos_conf,CLI_CONF_FOREIGN_LOGIN_ENABLE_KEY,&foreign_login_enable);
        vos_info_print("foreign_login_enable %ld\r\n",foreign_login_enable);

        VOS_config_get_number_quiet(vos_conf,CLI_CONF_TELNET_PORT_KEY,&cli_telnet_port);
        vos_info_print("cli_telnet_port %ld\r\n",cli_telnet_port);

        size = VOS_SYSNAME_LEN;
        VOS_config_get_string_quiet(vos_conf,CLI_CONF_HOST_NAME_KEY,vos_hostname,&size);
        vos_info_print("vos_hostname %s\r\n",vos_hostname);

        size = VOS_PRODUCT_NAME_LEN;
        VOS_config_get_string_quiet(vos_conf,CLI_CONF_PRODUCT_NAME_KEY,vos_product_name,&size);
        vos_info_print("vos_product_name %s\r\n",vos_product_name);

        VOS_config_vos_file_close();
        return VOS_OK;
    }

    return VOS_ERROR;
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


    VOS_StrCpy(cli_module.name,MOD_NAME_VOS_CLI);


    ret= VOS_module_register(cli_module.name,&cli_module);
    if(VOS_OK != ret)
    {
        VOS_ASSERT(0);
        cli_err_print("cli mod reg err %d",ret);
    }

    g_cli_lock = VOS_SemMCreate(VOS_SEM_Q_FIFO);
    if(!g_cli_lock)
    {
        vos_err_print("create cli lock error\r\n");
        VOS_ASSERT(0);
        return VOS_ERROR;
    }


    vos_config_parse_cli();

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



#ifdef __cplusplus
}
#endif

