#ifndef _CLI_H_
#define _CLI_H_

#include "config.h"
//#include "cli.h"
//#include "cli_vty.h"

#define _VOS_CLI_MAX_MSG_QUEUE_LEN_   256
#define  CLI_AS_MANAGER_MSG_QUE_CHAN_ID 0 /* 命令行作为设备管理模块钻用的消息通道号*/

#define VOS_MSG_TYPE_QUE    1
#define VOS_MSG_TYPE_CHAN    2

#define VTY_BUFSIZ 512
#define VTY_OUT_BUFSIZ 1024


#ifndef TO_STR
#define TO_STR(m) #m
#endif

#ifndef INT_TO_STR
#define INT_TO_STR(m) TO_STR(m)
#endif

#define CMD_COMMAND(cmd) (cmd.string)



#define CMD_MESGTYPE_CMD    ( (VOS_MOD_CLI << 16) + 1 )

/***********************************************************************

 part I : vty struct and function

***********************************************************************/ 



#define _CL_VTY_OUT_SEM_WAIT_TIME_           (-1)

struct vty_ftp_para
{
    VOS_HANDLE hlFtpClientTaskID;
    ULONG ulQueID;
    CHAR chReturnShellNode;
    LONG lnCurSubNode;
};


/* Prototypes. */
int vty_big_out (struct vty *vty, int maxlen,const char *format, ...);
int vty_direct_out (struct vty *vty , const char *format, ...);

//int cl_vty_assert_out(const char *format, ...);


#define vty_printf( fmd... ) vty_out( vty, ##fmd )

/******************************************************
funcname : cl_vty_monitor_out
description : used for display debug info 
input args : just like VOS_Printf
return values : 1, success
              0, failed
*****************************************************/
int cl_vty_monitor_out(const char *format, ...);
/* 这两个函数为大量的数据输出使用,例如show fib
这个函数会阻塞住命令行的任务,自己做分屏输出,
和处理用户的任意键输入ctrl+c,必须用在DEFUN中
同时使用者需要判断此函数的返回值,如果小于0
就直接从DEFUN中返回
注意:使用的时候需要先调用vty_very_big_out_init ,之后连续
调用vty_very_big_out*/
void vty_very_big_out_init ( struct vty *vty );
int vty_very_big_out (struct vty *vty, const char *format, ...);

/* add for the INTERFACE_NODE,VLAN_NODE,SUPER_VLAN_NODE,TRUNK_NODE,TUNNEL_NODE,LSP_NODE,
LOOPBACK_NODE,POS_NODE prompt when two or more user in the switch by liuyanjun 2002/10/14 */
void vty_set_prompt(struct vty *vty, char *cpPrompt);

/**********************************
funcname: cl_vty_freeze
description: used for freeze vty's in and out
           (maybe in or out in future)
           make a vty didn't recieve any input 
           output only with vty_direct_out
           used when a command can't return immediately
************************************/           
int cl_vty_freeze(void *arg,int frozen_type);


/**********************************
funcname: cl_vty_unfreeze
description: used for unfreeze vty's in and out
           (maybe in or out in future)
           used when a command finished and return .
************************************/           
int cl_vty_unfreeze(void *arg,int unfrozen_to);

/****************************************************************

Part II : Command struct and function

*******************************************************************/

/* There are some command levels which called from command node. */



#define CMD_COMMAND(cmd) (cmd.string)
#define CMD_HELPINFO(cmd) (cmd.doc)


/* Return value of the commands. */
#define CMD_SUCCESS              0
#define CMD_WARNING              1
#define CMD_ERR_NO_MATCH         2
#define CMD_ERR_AMBIGUOUS        3
#define CMD_ERR_INCOMPLETE       4
#define CMD_ERR_EXEED_ARGC_MAX   5
#define CMD_ERR_NOTHING_TODO     6
#define CMD_COMPLETE_FULL_MATCH  7
#define CMD_COMPLETE_MATCH       8
#define CMD_COMPLETE_LIST_MATCH  9
#define CMD_SUCCESS_DAEMON      10


#define DescStringCommonCreate "Create system's setting\n"
#define DescStringCommonDelete "Delete system's setting\n"
#define DescStringCommonConfig "Config system's setting\n"
#define DescStringCommonShow "Show running system information\n"
#define DescStringCommonService "Configure system services\n"


#define QDEFUN_FLAG   1
#define MDEFUN_FLAG   2
#define LDEFUN_FLAG   3
#define PDEFUN_FLAG   4

#define QDEFUN(funcname, cmdname, cmdstr, helpstr,queid)   \
	int funcname (struct cmd_element *, struct vty *, int, char **);\
	struct cmd_element cmdname =   {cmdstr,     funcname,     helpstr ,QDEFUN_FLAG,(unsigned long)(queid) };\
	int funcname  (struct cmd_element *self, struct vty *vty, int argc, char **argv)
#define MDEFUN(funcname, cmdname, cmdstr, helpstr,moduleid)   \
	int funcname (struct cmd_element *, struct vty *, int, char **);\
	struct cmd_element cmdname =   {cmdstr,     funcname,     helpstr ,MDEFUN_FLAG,moduleid };\
	int funcname  (struct cmd_element *self, struct vty *vty, int argc, char **argv)

#define LDEFUN(funcname, cmdname, cmdstr, helpstr,nodeid)   \
	int (ldef_ ## funcname) (struct cmd_element *, struct vty *, int, char **);\
	struct cmd_element (ldef_ ## cmdname) =   {cmdstr,     (ldef_ ## funcname),     helpstr ,LDEFUN_FLAG,nodeid};\
	int (ldef_ ## funcname)  (struct cmd_element *self, struct vty *vty, int argc, char **argv)

#define LDEFUN_INSTALL_ELEMENT(ntype, cmd)\
    VOS_cli_install_element(ntype,&(ldef_ ## cmd))    

#define PDEFUN(funcname, cmdname, cmdstr, helpstr)   \
	int funcname (struct cmd_element *, struct vty *, int, char **);\
	struct cmd_element cmdname =   {cmdstr,     funcname,     helpstr ,PDEFUN_FLAG};\
	int funcname  (struct cmd_element *self, struct vty *vty, int argc, char **argv)

#if 0
/* DEFUN for vty command interafce. Little bit hacky ;-). */
#define DEFUN(funcname, cmdname, cmdstr, helpstr)   int funcname (struct cmd_element *, struct vty *, int, char **);   struct cmd_element cmdname =   {     cmdstr,     funcname,     helpstr   };   int funcname  (struct cmd_element *self, struct vty *vty, int argc, char **argv)  
/* DEFSH for vtysh. */
#define DEFSH(daemon, cmdname, cmdstr, helpstr)  struct cmd_element cmdname =   {     cmdstr,     NULL,     helpstr,    daemon   }; 
/* DEFUN + DEFSH */
#define DEFUNSH(daemon, funcname, cmdname, cmdstr, helpstr)   int funcname (struct cmd_element *, struct vty *, int, char **);   struct cmd_element cmdname =   {     cmdstr,     funcname,     helpstr,     daemon   };   int funcname  (struct cmd_element *self, struct vty *vty, int argc, char **argv)
#endif

/* ALIAS macro which define existing command's alias. */
#define ALIAS(funcname, cmdname, cmdstr, helpstr)   struct cmd_element cmdname =   {     cmdstr,     funcname,     helpstr,DEFUN_FLAG};
#define QALIAS(funcname, cmdname, cmdstr, helpstr,queid)   struct cmd_element cmdname =   {     cmdstr,     funcname,     helpstr ,QDEFUN_FLAG,queid};
#define MALIAS(funcname, cmdname, cmdstr, helpstr,moduleid)   struct cmd_element cmdname =   {     cmdstr,     funcname,     helpstr ,MDEFUN_FLAG,moduleid};


/* Some macroes */

/* Common descriptions. */

#define IP_STR "IP information\n"
#define IPV6_STR "IPv6 information\n"
#define CLEAR_STR "Reset functions\n"
#define RIP_STR "RIP information\n"
#define BGP_STR "BGP information\n"
#define EIGRP_STR "EIGRP information\n"
#define OSPF_STR "OSPF information\n"
#if (RPU_MODULE_ISIS == RPU_YES)
#define ISIS_STR "ISIS information\n"
#endif/*RPU_MODULE_ISIS*/    
#define NEIGHBOR_STR "Specify neighbor router\n"
#define ROUTER_STR "Enable a routing process\n"
#define AS_STR "AS number\n"
#define MBGP_STR "MBGP information\n"
#define MATCH_STR "Match values from routing table\n"
#define SET_STR "Set values in destination routing protocol\n"
#define OUT_STR "Filter outgoing routing updates\n"
#define IN_STR  "Filter incoming routing updates\n"
#define V4NOTATION_STR "specify by IPv4 address notation(e.g. 0.0.0.0)\n"
#define OSPF6_NUMBER_STR "Specify by number\n"
#define INTERFACE_STR "Interface information\n"
#define IFNAME_STR "Interface name\n"
#define IP6_STR "IPv6 Information\n"
#define OSPF6_STR "Open Shortest Path First (OSPF) for IPv6\n"
#define OSPF6_ROUTER_STR "Enable a routing process\n"
#define OSPF6_INSTANCE_STR "<1-65535> Instance ID\n"
#define SECONDS_STR "<1-65535> Seconds\n"
#define ROUTE_STR "Routing Table\n"
#define PREFIX_LIST_STR "Prefix list information\n"
#define OSPF6_DUMP_TYPE_LIST "(hello|dbdesc|lsreq|lsupdate|lsack|neighbor|interface|area|lsa|cl_lib|config|dbex|spf|route|lsdb|redistribute)"

#define ICMP_STR "ICMP information\n"
#define UDP_STR "UDP information\n"
#define TCP_STR "TCP information\n"


/* Export typical functions. */

int config_exit (struct cmd_element *, struct vty *, int, char **);
int config_help (struct cmd_element *, struct vty *, int, char **);

/*************************************************************

Part III: command user module's struct and function

**************************************************************/

struct cl_cmd_module
{
    char *module_name;		/* module's name */
    int (*init_func) ();		/*  module's init function , mainly install element; */
    /* modules's show run function,
     mainly show every module's current running configuration */
    int (*showrun_func) (struct vty *);
    struct cl_cmd_module *prev;	/*  previous pointer of module; */
    struct cl_cmd_module *next;	/* next pointer of module; */
};

/* 在主任务中添加对命令行多任务的监视,包括串口的子任务和其他的telnet的子任务
   有子任务死掉或者没有响应,就杀掉这个子任务,如果是串口就重新启动串口的描述符
   用于再次登陆 by liuyanjun 2002/07/06 */
/* the time unit is second ,the default value is 1 day */
#define CL_TASK_DEAD_TIME	 60*60*24
/* 因为每个用户最多可以有10个telnet，最多可以有4个用户同时登陆*/
#define CL_TELNET_MAX		40  

typedef struct _cliTaskActive
{
 int active;			/* the task is or not dead ,TRUE : live, FALSE: dead */
 VOS_HANDLE taskHandle;		/* the task VOS_HANDLE */
 int secondCount;		/* the task not response count, unit: 1 second */
 void *taskVty;			/* the task vty */
 CHAR cliTaskName[20];	/* the task name */
 int userFlag;			/* TRUE: use, FALSE: not use*/
}cliTaskActive;

extern cliTaskActive consoleTaskActive;
extern cliTaskActive telnetTaskActive[CL_TELNET_MAX];

void cliSetTaskFlagWhenTaskCreate(void *taskvty);
void cliSetTaskNoActiveAfterSelect();
void cliCheckSetActiveTaskAfterSelect();
/*   但此时忽略了串口和telnet的idletimeOut 情况,在idletimeout时这个命令行的子任务已经被删掉了
   所以在idletimeout时加入清除任务状态的函数调用*/
void cliClearActiveTaskWhenVtyClose();
LONG  vty_exit_node(struct vty *vty);


#define cli_err_print(_format, ...)  {VOS_Printf(VOS_COLOR_RED"%s.%d:",__func__,__LINE__);VOS_Printf(_format, ##__VA_ARGS__) ;VOS_Printf(VOS_COLOR_NONE" ");}


//extern vos_module_t cli_module;
extern LONG g_cli_lock;

#define MODULE_RPU_CLI          cli_module.moduleID
#define vos_cli_lock()          VOS_SemTake(g_cli_lock,VOS_WAIT_FOREVER)
#define vos_cli_unlock()        VOS_SemGive(g_cli_lock)
#define VOS_PRODUCT_NAME_LEN (128)

#endif /* _CLI_H */ 

