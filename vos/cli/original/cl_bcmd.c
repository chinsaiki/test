/**
 * @file cl_bcmd.c
 * @brief 定义基本命令
 * @details 定义基本命令
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

#include "cli/cli.h"
#include "cli/cl_vty.h"
#include "cli/cl_cmd.h"
#include "cli/cl_vect.h"
#include "cli/cl_buf.h"
#include "cli/cl_set.h"

#include "cli/cli_product_adapter.h"

/*
 * Command cl_vector which includes some level of command lists. Normally each
 * daemon maintains each own cmdvec. defined in cl_cmd.c
 */
extern cl_vector cl_cmdvec_master;

/* defined in cl_vty.c */
extern cl_vector cl_vtyvec_master;

extern int cl_vty_session_count;
extern int cl_vty_user_limit;


/*
 * Standard command node structures. 
 
 */

struct cmd_node view_node =
    {
        VOS_CLI_VIEW_NODE,
        ">",
    };

struct cmd_node config_node =
    {
        VOS_CLI_CONFIG_NODE,
        "(config)#",
    };

struct cmd_node cl_debug_hidden_node =
    {
        VOS_CLI_DEBUG_HIDDEN_NODE,
        "(DEBUG_H)> ",
    };


/************************************************************
  Cli related Basic command 
 
************************************************************/


/*
 * Enable command 
 */
DEFUN( enable,
       config_enable_cmd, "enable", "Turn on privileged mode command\n" )
{
    vty->node = VOS_CLI_CONFIG_NODE;

    return CMD_SUCCESS;

}


/*
 * Down vty node level. 
 */
DEFUN ( config_exit,
        config_exit_cmd,
        "exit", "Exit current mode and back to previous mode\n" )
{
    switch ( vty->node )
    {
        case VOS_CLI_VIEW_NODE:
            {
                vty_out( vty, "Exit.%s", VTY_NEWLINE );
                vty->status = VTY_CLOSE;
            }
            break;

        case VOS_CLI_CONFIG_NODE:
            {
                vty->node = VOS_CLI_VIEW_NODE;
                vty_config_unlock ( vty );
            }

            break;

        case VOS_CLI_DEBUG_HIDDEN_NODE:
            {
                vty->node = vty->prev_node;
            }
            break;
        
    	default:
            {
                vty_exit_node(vty);
            }
            break;
    }

    if(vty->context_data && vty->context_data_need_free)
    {
        VOS_Free(vty->context_data);
        vty->context_data = NULL;
        vty->context_data_need_free = 0;
    }

    return CMD_SUCCESS;
}

DEFUN ( config_quit,
        config_quit_cmd, "quit", "Disconnect from switch and quit\n" )
{
    switch(vty->node)
    {
        default:
    vty_out( vty, "Quit.%s", VTY_NEWLINE );
    vty->status = VTY_CLOSE;
            break;
    }

    return CMD_SUCCESS;
}
ALIAS ( config_quit,
        config_logout_cmd,
        "logout",
        "Disconnect from switch and quit\n" )

/*
 * Show version. 
 */

DEFUN ( show_version,
        show_version_cmd,
        "show version", 
        SHOW_STR 
        "Display GROS version\n" )
{
    vty_out( vty, "\r\n  show version\r\n");

    return CMD_SUCCESS;
}


/*
 * Help display function for all node.
 */
DEFUN ( config_help,
        config_help_cmd,
        "help", "Description of the interactive help system\n" )
{
    vty_out( vty, "\r\n" );
    vty_out( vty, "      GROS provides help feature as described below.\r\n\r\n" );

    vty_out( vty, "      1. Anytime you need help, just press \"?\" and don\'t\r\n" );
    vty_out( vty, "  press Enter, you can see each possible command argument\r\n" );
    vty_out( vty, "  and its description.\r\n\r\n" );
    vty_out( vty, "      2. You can also input \"list\" and then press Enter\r\n" );
    vty_out( vty, "  to execute this helpful command to view the list of\r\n" );
    vty_out( vty, "  commands you can use.\r\n\r\n" );

    return CMD_SUCCESS;
}

/*
 * Help display function for all node. 
 */
DEFUN ( config_list, config_list_cmd, "list", "Print command list\n" )
{
    unsigned int i;
    struct cmd_node *cnode = cl_vector_slot ( cl_cmdvec_master, vty->node );
    struct cmd_element *cmd;

    for ( i = 0; i < cl_vector_max ( cnode->cmd_vector ); i++ )
    {
        if ( ( cmd = cl_vector_slot ( cnode->cmd_vector, i ) ) != NULL )
        {
            vty_out ( vty, "  %s%s", cmd->string, VTY_NEWLINE );
        }
    }

    return CMD_SUCCESS;
}

/* FUNCTION : list the command including string<pattern> in current node   */
DEFUN ( config_list_patten_func,
        config_list_patten_cmd,
        "list <string>",
        "Print command list\n"
        "String to be matched\n" )
{
    unsigned int i;
    struct cmd_node *cnode = cl_vector_slot ( cl_cmdvec_master, vty->node );
    struct cmd_element *cmd;

    for ( i = 0; i < cl_vector_max ( cnode->cmd_vector ); i++ )
    {
        if ( ( cmd = cl_vector_slot ( cnode->cmd_vector, i ) ) != NULL )
        {
            if ( VOS_StrStr( cmd->string, argv[ 0 ] ) != NULL )
            {
                vty_out ( vty, "  %s%s", cmd->string, VTY_NEWLINE );
            }
        }
    }
    return CMD_SUCCESS;
}

DEFUN( show_sysinfo,
       show_sysinfo_cmd,
       "show sysinfo",
       SHOW_STR
       "Show system's info\n" )
{
    char * p = NULL;

    vty_out( vty, "  ------------------------------------------\r\n");
    
    p = VOS_get_hostname();
    vty_out( vty, "  host name   :  %s%s", p, VTY_NEWLINE );

    p = VOS_get_product_name();
    vty_out( vty, "  product name:  %s%s", p, VTY_NEWLINE );
    
    vty_out( vty, "  ------------------------------------------\r\n");


    return CMD_SUCCESS;
}

DEFUN ( config_no_terminal_length, config_no_terminal_length_cmd,
        "undo screen lines",
        NO_STR
        "Set number of lines to default value\n"
        "Set number of lines to default value\n" )
{
    vty->lines = _CL_VTY_TERMLENGTH_DEFAULT_;

    return CMD_SUCCESS;
}

DEFUN ( show_terminal_length, show_terminal_length_cmd,
        "show screen lines",
        SHOW_STR
        "Show screen line parameters\n"
        "Show number of lines to default value\n" )
{
	/*ID 2758 terminal length命令的显示结果没有修改*/
    vty_out( vty, "  This screen line length is %d\r\n", ( vty->lines == 0 ) ? 0 : ( vty->lines - 2 ) );

    return CMD_SUCCESS;
}


DEFUN ( config_terminal_length, config_terminal_length_cmd,
        "screen lines <0-512>",
        "Set screen parameters\n"
        "Set number of lines on a screen\n"
        "Number of lines on a screen (0 for no pausing)\n" )
{
    int lines;
    char *endptr = NULL;

    lines = ( int ) VOS_StrToUL( argv[ 0 ], &endptr, 10 );
    if ( lines < 0 || lines > 512 || *endptr != '\0' )
    {
        vty_out ( vty, "%% Error, Length is malformed%s", VTY_NEWLINE );
        return CMD_WARNING;
    }
    if ( lines == 0 )
    {
        vty->lines = 0;
    }
    else
    {
        vty->lines = lines + 2;
    }

    return CMD_SUCCESS;
}



DEFUN ( config_kill_session,
        config_kill_session_cmd,
        "kill session <1-19999>",
        "Kill some unexpected things\n"
        "Kill some unexpected telnet session\n"
        "Input the session id you want to kill (This id you can see by using who command)\n" )
{
    int kill_id;
    kill_id = VOS_AtoI( argv[ 0 ] );

    if ( kill_id != vty->fd )
    {
        cl_vty_close_by_sessionid( vty, kill_id );
    }
    else
    {
        if ( vty->conn_type == _CL_VTY_CONSOLE_ && cl_vty_user_limit == 1 )
        {
            vty_out( vty, "%% Error, You can't close a console session.\r\n" );
        }
        else
        {
            if ( vty->user_name == NULL || vty->user_name[ 0 ] == ' ' ) /*没有的认证登录方式*/
            {
                vty_out( vty, "  %sYou are closed by %s.%s", VTY_NEWLINE, "other user", VTY_NEWLINE );
            }
            else
            {
                vty_out( vty, "  %sYou are closed by %s.%s", VTY_NEWLINE, vty->user_name, VTY_NEWLINE );
            }
            vty->status = VTY_CLOSE;
        }
    }

    return CMD_SUCCESS;
}

DEFUN ( config_who, config_who_cmd, "who",
        "Display who is connected to the switch\n" )
{
    unsigned int i;
    struct vty *v;

    vty_out( vty, "SessionID. - UserName ---------- LOCATION ---------- MODE ---- %s", VTY_NEWLINE );

    for ( i = 0; i < cl_vector_max ( cl_vtyvec_master ); i++ )
    {
        if ( ( v = cl_vector_slot ( cl_vtyvec_master, i ) ) != NULL )
        {
            vty_out( vty, "%-13d%-20s%-20s%-10s%s%s",
                     v->fd,
                     v->user_name ? v->user_name : "not login",
                     v->address,
                     v->user_name ? ( v->config ? "CONFIG" : "VIEW" ) : "not login",
                             v == vty ? "(That's me.)" : " ",
                             VTY_NEWLINE );
        }
    }

    vty_out( vty, "Total %d sessions in current system. %s", cl_vty_session_count, VTY_NEWLINE );

    return CMD_SUCCESS;
}

DEFUN ( config_whoami, config_whoami_cmd,
        "who am i ",
        "Display who is connected to the switch\n"
        "Display myself who is connected to the target machine\n"
        "Display only myself\n" )
{
    unsigned int i;
    struct vty *v;

    for ( i = 0; i < cl_vector_max ( cl_vtyvec_master ); i++ )
    {
        if ( ( v = cl_vector_slot ( cl_vtyvec_master, i ) ) == vty )
        {
            vty_out ( vty, "  I am %sSession [%d] : user %s connected from %s.%s",
                      v->config ? "*" : " ", i, v->user_name, v->address, VTY_NEWLINE );
        }
    }

    return CMD_SUCCESS;
}

DEFUN ( show_history,
        show_history_cmd,
        "history", "Display the session command history\n" )
{
    int index;
    int i;

    for ( index = vty->hindex, i = 0; i < VTY_MAXHIST; i++ )
    {
        if ( index == VTY_MAXHIST )
        {
            index = 0;
        }

        if ( vty->hist[ index ] != NULL )
        {
            vty_out ( vty, "  %s%s", vty->hist[ index ], VTY_NEWLINE );
        }

        index++;
    }

    return CMD_SUCCESS;
}

DEFUN ( cl_exec_timeout_show_func,
        cl_exec_timeout_show_cmd,
        /*"show idle-timeout",*/
	"show screen-idle-timeout",
        SHOW_STR
        "Idle timeout value in minutes and seconds\n" )
{

    return cl_exec_timeout_show ( vty );
}

DEFUN ( exec_no_timeout_min,
        exec_no_timeout_min_cmd,
        "undo screen-idle-timeout",
        NO_STR
        "Set Idle timeout default value\n" )
{
    char time_str[ 50 ];

    VOS_Sprintf( time_str, "%d", _CL_VTY_TIMEOUT_DEFAULT_ );
    return cl_exec_timeout ( vty, NULL, time_str );
}

DEFUN ( exec_timeout_min,
        exec_timeout_min_cmd,
        "screen idle-timeout <0-35791>",
        "Set screen parameters\n"
        "Set idle timeout value\n"
        "Idle timeout value in minutes (set to 0 will disable idle-timeout)\n" )
{
        return cl_exec_timeout ( vty,NULL , argv[ 0 ] );
   // return cl_exec_timeout ( vty, argv[ 0 ], NULL );
}


DEFUN( cl_clear_screen_func,
       cl_clear_screen_cmd,
       "clear",
       "Clear screen\n" )
{
    cl_vty_clear_screen( vty );
    return CMD_SUCCESS;
}


DEFUN ( terminal_monitor_func,
    terminal_monitor_cmd,
    "screen monitor",
    "screen display config\n"
    "Monitor\n" )
{

    if ( vty->debug_monitor == 1 )
    {
        vty_out( vty, "%% Terminal monitors already.\r\n" );
        return CMD_SUCCESS;
    }
    else
    {
        vty->debug_monitor = 1;
        cl_vty_monitor_count_inc();
    }

   return CMD_SUCCESS;
}

DEFUN ( no_terminal_monitor_func,
       no_terminal_monitor_cmd,
       "undo screen monitor",
       NO_STR
       "screen display config\n"
       "Monitor\n" )
{
    if ( vty->debug_monitor == 0 )
    {
        vty_out( vty, "%% Terminal doesn't monitor already.\r\n" );
        return CMD_SUCCESS;
    }
    else
    {
        vty->debug_monitor = 0;
        cl_vty_monitor_count_dec();
    }

   return CMD_SUCCESS;
}



/************************************************
  Install node default command 
 
***************************************************/
void install_default( enum node_type node )
{
    VOS_cli_install_element( node, &config_exit_cmd );

    VOS_cli_install_element( node, &config_quit_cmd );
    VOS_cli_install_element( node, &config_logout_cmd );
    VOS_cli_install_element( node, &show_history_cmd );
    VOS_cli_install_element( node, &cl_clear_screen_cmd );

    VOS_cli_install_element( node, &config_help_cmd );
    VOS_cli_install_element( node, &config_list_cmd );
    VOS_cli_install_element( node, &config_list_patten_cmd ); /* liujianxun add 2003-08-22 */
}


/************************************************
  Install all basic command 
 
***************************************************/

/** 
 * 安装基本命令
 * @return      成功返回 VOS_OK,失败返回其他
 */
int cl_install_basic_command()
{
    /*
    * Install top nodes. 
    */
    install_node( &view_node, NULL );
    install_default( VOS_CLI_VIEW_NODE );

    install_node( &config_node, NULL );
    install_default( VOS_CLI_CONFIG_NODE );

    install_node ( &cl_debug_hidden_node, NULL );
    install_default( VOS_CLI_DEBUG_HIDDEN_NODE );
    

    VOS_cli_install_element( VOS_CLI_VIEW_NODE, &config_enable_cmd );
    VOS_cli_install_element( VOS_CLI_VIEW_NODE, &config_who_cmd );
    VOS_cli_install_element( VOS_CLI_VIEW_NODE, &config_whoami_cmd );
    VOS_cli_install_element( VOS_CLI_VIEW_NODE, &cl_exec_timeout_show_cmd );
    VOS_cli_install_element( VOS_CLI_VIEW_NODE, &config_terminal_length_cmd );
    VOS_cli_install_element( VOS_CLI_VIEW_NODE, &config_no_terminal_length_cmd );
    VOS_cli_install_element( VOS_CLI_VIEW_NODE, &show_terminal_length_cmd );
    VOS_cli_install_element( VOS_CLI_VIEW_NODE, &show_sysinfo_cmd );


    VOS_cli_install_element( VOS_CLI_CONFIG_NODE, &show_sysinfo_cmd );
    VOS_cli_install_element( VOS_CLI_CONFIG_NODE, &config_terminal_length_cmd );
    VOS_cli_install_element( VOS_CLI_CONFIG_NODE, &config_no_terminal_length_cmd );
    VOS_cli_install_element( VOS_CLI_CONFIG_NODE, &show_terminal_length_cmd );
    VOS_cli_install_element( VOS_CLI_CONFIG_NODE, &config_who_cmd );
    VOS_cli_install_element( VOS_CLI_CONFIG_NODE, &config_whoami_cmd );
    VOS_cli_install_element( VOS_CLI_CONFIG_NODE, &cl_exec_timeout_show_cmd );
    VOS_cli_install_element( VOS_CLI_CONFIG_NODE, &exec_timeout_min_cmd );
    VOS_cli_install_element( VOS_CLI_CONFIG_NODE, &exec_no_timeout_min_cmd );
    VOS_cli_install_element( VOS_CLI_CONFIG_NODE, &config_kill_session_cmd );

    VOS_cli_install_element( VOS_CLI_DEBUG_HIDDEN_NODE, &terminal_monitor_cmd );
    VOS_cli_install_element( VOS_CLI_DEBUG_HIDDEN_NODE, &no_terminal_monitor_cmd );

    return VOS_OK;

}



#ifdef __cplusplus
}
#endif

