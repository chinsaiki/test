/**
 * @file cl_cmd.c
 * @brief cli 
 * @details 命令解析
 * @version 1.0
 * @author  wujianming  (wujianming@sylincom.com)
 * @date 2019.01.25
 * @copyright 
 *    Copyright (c) 2018 Sylincom.
 *    All rights reserved.
*/


#include "vos_global.h"

#ifdef __cplusplus
extern"C"
{
#endif

#include "vos_global.h"
#include "vos_common.h"
#include "vos/vospubh/vos_prv_time.h"

#include "lib/cl_lib_thread.h"

#include "vos/vospubh/vos_prv_sysmsg.h"


#include "cli/cl_set.h"
#include "cli/cl_cmd.h"
#include "cli/cl_vect.h"
#include "cli/cli_product_adapter.h"
#include "sys_main_api.h"
#ifdef PLAT_MODULE_SYSLOG
#include <plat_syslog.h>
#endif

int printf(const char *format, ...);

#define INIT_MATCHVEC_SIZE 10

#define DELAY_NOMAL_TICKS   3600    /* 默认值 */
ULONG   gul_DelayNomalTicks = DELAY_NOMAL_TICKS;

#define FIRST_MESSAGE_ID    1
#define MAX_TIME_OUT           (0XFFFFFFFF)

#define DECIMAL_STRLEN_MAX 10

extern int cl_serv_console_fd;
int VOS_console_fd;


struct _cmd_notify_register_array
{
    CMD_NOTIFY_REFISTER_S cmd_register[ CMD_NOTIFY_REG_NUM ] ;
    /*添加自注册类型的注释*/
    /*   match_str                      func
    1. <XXX.XXX.XXX>                   notify_cmd
    2. <slotportlist>                  
    */
    LONG num;                  /*注册的总数*/
}
g_cmd_notify_register_array ;


enum tag_Conflict
{
    CONFLICT_NO = 0,
    CONFLICT_YES = 1,
    CONFLICT_MATCH_FIRST = 2,
    CONFLICT_MATCH_SECOND = 3,
    CONFLICT_AMBIGUOUS = 4
};

enum tag_ExactMatch
{
    EXACT_MATCH_NO = 0,
    EXACT_MATCH_FIRST = 2,
    EXACT_MATCH_SECOND = 3
};

enum tag_Which
{
    WHICH_MATCH_NO = 0,
    WHICH_MATCH_FIRST = 1,
    WHICH_MATCH_SECOND = 2,
    WHICH_MATCH_BOTH = 3
};

enum tab_MatchWord
{
    MATCH_WORD_NO = 0,
    MATCH_WORD_VARIABLE = 1,
    MATCH_WORD_PARTLY = 2,
    MATCH_WORD_EXACT =3
};

static LONG vos_cli_node_id_max = VOS_CLI_RESV_MAX_NODE - 1;

cl_vector cl_cmdvec_master;

cl_vector cl_vector_copy ( cl_vector v );
enum match_type cmd_piece_match ( CHAR * cmd, CHAR * str );
enum match_type cmd_compare_paragraph (struct vty *vty, cl_vector vline, cl_vector cmd_vec, LONG vstart_index,
                                        LONG cmd_startindex, LONG cmp_length, LONG vline_complete,
                                        LONG * argc, CHAR **argv, LONG * nTrueVarag ) ;

void __printf_cmd(struct cmd_element * cmd)
{
    int i = 0,j = 0;
    struct subcmd *subcmd = NULL;
    printf("str %s\n",cmd->string);
    printf("doc %s\n",cmd->doc);
    for ( i = 0; i < cl_vector_max ( cmd->strvec ); i++ )
    {
        cl_vector pstrvec = cl_vector_slot(cmd->strvec,i);
        if(pstrvec)
        {
            for ( j = 0; j < cl_vector_max (pstrvec ); j++ )
            {
                struct desc *desc = cl_vector_slot(pstrvec,j);
                printf("desc->cmd   \t--%s\n",desc->cmd);
                printf("desc->str   \t--%s\n",desc->str);
                printf("desc->multi \t--%d\n",desc->multi);
                printf("desc->argpos\t--%d\n",desc->argpos);
                printf("-----------\n");
            }
        }
        printf("111111111111\n");
    }
    
    if(cmd->subconfig)
    {
        for ( i = 0; i < cl_vector_max ( cmd->subconfig ); i++ )
        {
            subcmd = cl_vector_slot(cmd->subconfig,i);
            if(subcmd)
            {
                printf("subcmd->head_index\t--%d\n",subcmd->head_index);
                printf("subcmd->tail_index\t--%d\n",subcmd->tail_index);
                printf("subcmd->min_times \t--%d\n",subcmd->min_times);
                printf("subcmd->max_times \t--%d\n",subcmd->max_times);
                printf("subcmd->permute   \t--%d\n",subcmd->permute);
                printf("subcmd->subconfig \t--%p\n",subcmd->subconfig);
                printf("=========\n");
            }
        }
    }
    printf("===================================================\n");
}

void __printf_cmd_node(struct cmd_node * node)
{
    int i = 0;
    
    printf("node id %d,%s \n",node->node_id,node->prompt);
    for ( i = 0; i < cl_vector_max ( node->cmd_vector ); i++ )
    {
        struct cmd_element * cmd = cl_vector_slot(node->cmd_vector,i);
        if(cmd)
        {
            __printf_cmd(cmd);
        }
    }
}
void printfCmdevc(void)
{
VOS_Printf("max %d,alloced %d,index %d\n",
	cl_cmdvec_master->max,cl_cmdvec_master->alloced,cl_cmdvec_master->index);
	{
        int i = 0;
        for ( i = 0; i < cl_vector_max ( cl_cmdvec_master ); i++ )
        {
            struct cmd_node * node = cl_vector_slot(cl_cmdvec_master,i);
            if(node)
            {
                __printf_cmd_node(node);
            }
            printf("###################################################\n");
        }
	}
}

LONG cmd_cmdsize ( cl_vector strvec );

/*对外提供的注册机制*/
enum match_type cmd_rugular_register( CMD_NOTIFY_REFISTER_S * pNotifyRegister )
{
    LONG index = 0 ;
    LONG nCount = 0;
    CHAR *match_str = NULL ;
    LONG nLen = 0;
    if ( pNotifyRegister == NULL )
    {
        return no_match;
    }
    if ( pNotifyRegister->match_str == NULL || pNotifyRegister->pnotify_call == NULL )
    {
        return no_match;
    }
    match_str = pNotifyRegister->match_str ;
    nLen = VOS_StrLen( match_str );
    if ( nLen <= 2 )
    {
        return no_match;
    }
    if ( match_str[ 0 ] != '<' || match_str[ nLen - 1 ] != '>' )
    {
        return no_match;
    }
    for ( index = 1; index < nLen - 1 ; index++ )
    {
        if ( match_str[ index ] == '<' || match_str[ index ] == '>' )
        {
            return no_match;
        }
    }

    nCount = g_cmd_notify_register_array.num;
    if ( nCount >= CMD_NOTIFY_REG_NUM )
    {
        VOS_ASSERT(0); /*没有可以注册的空间了*/
        return no_match ;
    }
    for(index = 0; index < nCount; index++)
    {
        if(0 == VOS_StrCmp(g_cmd_notify_register_array.cmd_register[ index ].match_str, match_str))
        {
            VOS_ASSERT(0); /*已经注册过了*/
            return no_match ;
        }
    }
    g_cmd_notify_register_array.cmd_register[ nCount ].match_str = pNotifyRegister->match_str ;
    g_cmd_notify_register_array.cmd_register[ nCount ].pnotify_call = pNotifyRegister->pnotify_call;
    g_cmd_notify_register_array.cmd_register[ nCount ].IsCapital = pNotifyRegister->IsCapital;
    g_cmd_notify_register_array.num ++ ;

    return register_match ;

}


/* compare one paragraph with permutation */
/* sujiang,2002-08-15 */
enum match_type cmd_permute_partly_compare_paragraph ( cl_vector vline, cl_vector cmd_vec, LONG vstart_index,
        LONG cmd_startindex, LONG cmp_length, CHAR **match_str, LONG * real_compare_length, cl_vector match_item_vec, LONG * last_item_index )
{
    #if 0
    enum match_type match = no_match;
    enum match_type match_inloop = no_match;
    enum match_type result = no_match;
    enum match_type ret = no_match;
    LONG i, j;
    cl_vector descvec;
    cl_vector match_str_vec;
    LONG match_str_len;
    ULONG cmd_index;
    struct desc *desc;
    LONG end = 0;
    CHAR *flags = NULL;
    LONG matched = 0;
    CHAR *return_match_str = NULL;
    CHAR *temp_match_str = NULL;
    LONG item_index = 0;
    LONG oldvstart_index = vstart_index;

    if ( last_item_index )
    {
        *last_item_index = 0;
    }
    flags = VOS_Malloc( cmp_length, MODULE_RPU_CLI );
    if ( flags == NULL )       /* add by liuyanjun 2002/08/26 */
    {
        ASSERT( 0 );
        return no_match;
    }                       /* end */

    for ( i = 0; i < cmp_length; i++ )
    {
        flags[ i ] = 0;
    }

    match_str_vec = cl_vector_init( INIT_MATCHVEC_SIZE );
    if ( NULL == match_str_vec )
    {
        ASSERT( 0 );
        VOS_Free( flags );
        return no_match;
    }

    while ( !end )
    {
        match_inloop = no_match;
        item_index = 0;
        for ( i = 0; i < cmp_length; i++ )
        {
            cmd_index = cmd_startindex + i;
            descvec = cl_vector_slot ( cmd_vec, cmd_index );
            desc = cl_vector_slot( descvec, 0 );
            /* 如果输入命令中已经没有命令分片。 */
            if ( vstart_index >= ( LONG ) cl_vector_max( vline ) )
            {
                end = 1;
                break;
            }
            if ( !cl_vector_slot( vline, vstart_index ) )
            {
                end = 1;
                break;
            }
            /* 如果该节点已经找到匹配的输入。 */
            if ( flags[ i ] )
            {
                item_index += descvec->max;
                continue;
            }
            /* 如果所有的节点已经全部匹配。 */
            if ( matched == cmp_length )
            {
                end = 1;
                break;
            }
            /*
            cmd_index = cmd_startindex + i;
            descvec = cl_vector_slot (cmd_vec, cmd_index);
            desc = cl_vector_slot( descvec, 0 );*/
            if ( desc->multi == 3 )
            {
                ret = cmd_multi_partly_compare_one_node( vline, descvec, &vstart_index, match_str_vec, match_item_vec, item_index, last_item_index );
            }
            else
            {
                ret = cmd_permute_partly_compare_one_node( vline, descvec, &vstart_index, match_str_vec, match_item_vec, item_index, last_item_index );
            }

            item_index += descvec->max;
            match_inloop = ret;
            /* 如果匹配,退出循环。 */
            if ( ret )
            {
                if ( ret != incomplete_match )
                {
                    flags[ i ] = 1;
                    matched++;
                    match = ret;
                }
                else
                {
                    matched = 0;
                    match = no_match;
                    end = 1;
                }
                break;
            }
            /* 如果所有的节点都不跟输入命令匹配。 */
            else if ( !ret && ( i == cmp_length - 1 ) )
            {
                end = 1;
                break;
            }
        }

        /* 输入的命令没有匹配的节点。 */
        if ( !match_inloop )
        {
            end = 1;
        }
    }

    VOS_Free( flags );

    *real_compare_length = vstart_index - oldvstart_index;

    result = match;

    goto check_cl_vector;

check_cl_vector:
    if ( result == no_match )   /* no match , free all match_str_vec */
    {
        /*
        * Free matchvec.
        */
        for ( i = 0; i < ( LONG ) cl_vector_max ( match_str_vec ); i++ )
        {
            if ( cl_vector_slot ( match_str_vec, i ) )
            {
                VOS_Free ( cl_vector_slot ( match_str_vec, i ) );
            }
        }
        cl_vector_free ( match_str_vec );

        *match_str = NULL;
    }
    else   /* match, copy match_str_vec to a string */
    {
        /*
        * Get Match_str_total_len
        */
        match_str_len = 0;
        for ( i = 0; i < ( LONG ) cl_vector_max ( match_str_vec ); i++ )
        {
            if ( cl_vector_slot ( match_str_vec, i ) )
            {
                match_str_len = match_str_len + VOS_StrLen( ( CHAR * ) cl_vector_slot( match_str_vec, i ) );
            }
        }

        /*
        * Malloc for retun_match_str *
        */
        return_match_str = ( CHAR * ) VOS_Malloc( ( match_str_len + 1 ), MODULE_RPU_CLI );
        if ( return_match_str == NULL )       /* add by liuyanjun 2002/08/26 */
        {
            ASSERT( 0 );
            return no_match;
        }                       /* end */


        /*
        * Copy match_str_cl_vector LONGo one string
        */
        match_str_len = 0;
        for ( i = 0; i < ( LONG ) cl_vector_max ( match_str_vec ); i++ )
        {
            if ( ( temp_match_str = cl_vector_slot ( match_str_vec, i ) ) != NULL )
            {
                for ( j = 0; j < ( LONG ) VOS_StrLen( temp_match_str ); j++ )
                {
                    return_match_str[ match_str_len + j ] = temp_match_str[ j ];
                }
                match_str_len = match_str_len + VOS_StrLen( temp_match_str );
            }
        }

        return_match_str[ match_str_len ] = '\0';

        *match_str = return_match_str;
        /*
        * Free matchvec.
        */
        for ( i = 0; i < ( LONG ) cl_vector_max ( match_str_vec ); i++ )
        {
            if ( cl_vector_slot ( match_str_vec, i ) )
            {
                VOS_Free ( cl_vector_slot ( match_str_vec, i ) );
            }
        }
        cl_vector_free ( match_str_vec );
    }

    return result;
    #endif
    return no_match;
}

/* compare one paragraph with permutation */
/* sujiang,2002-08-15 */
enum match_type cmd_permute_compare_paragraph ( cl_vector vline, cl_vector cmd_vec, LONG vstart_index,
        LONG cmd_startindex, LONG cmp_length, LONG * argc,
        CHAR **argv, LONG * compare_length )
{
    #if 0
    enum match_type match = no_match;
    enum match_type match_inloop = no_match;
    enum match_type ret;
    LONG i;
    cl_vector descvec;
    ULONG cmd_index;
    struct desc *desc;
    LONG oldargc;
    LONG end = 0;
    LONG oldvstart_index = vstart_index;
    CHAR *flags = NULL;
    LONG matched = 0;

    oldargc = *argc;

    flags = VOS_Malloc( cmp_length, MODULE_RPU_CLI );
    if ( flags == NULL )       /* add by liuyanjun 2002/08/26 */
    {
        ASSERT( 0 );
        return no_match;
    }                       /* end */

    for ( i = 0; i < cmp_length; i++ )
    {
        flags[ i ] = 0;
    }

    while ( !end )
    {
        match_inloop = no_match;
        for ( i = 0; i < cmp_length; i++ )
        {
            /* 如果输入命令中已经没有命令分片。 */
            if ( vstart_index >= ( LONG ) cl_vector_max( vline ) )
            {
                end = 1;
                break;
            }
            /* 如果该节点已经找到匹配的输入。 */
            if ( flags[ i ] )
            {
                continue;
            }
            /* 如果所有的节点已经全部匹配。 */
            if ( matched == cmp_length )
            {
                end = 1;
                break;
            }
            cmd_index = cmd_startindex + i;
            descvec = cl_vector_slot ( cmd_vec, cmd_index );
            desc = cl_vector_slot( descvec, 0 );
            if ( desc->multi == 3 )
            {
                ret = cmd_multi_compare_one_node( vline, descvec, &vstart_index, argc, argv );
            }
            else
            {
                ret = cmd_permute_compare_one_node( vline, descvec, &vstart_index, argc, argv );
            }

            match_inloop = ret;
            /* 如果如果命令没有输入全 */
            if ( ret == incomplete_match || ret == ambiguous_match )
            {
                match = ret;
                end = 1;
                break;
            }
            /* 如果匹配,退出循环。 */
            else if ( ret )
            {
                flags[ i ] = 1;
                matched++;
                match = ret;
                break;
            }
            /* 如果所有的节点都不跟输入命令匹配。 */
            else if ( !ret && ( i == cmp_length - 1 ) )
            {
                end = 1;
                break;
            }
        }

        /* 输入的命令没有匹配的节点。 */
        if ( !match_inloop )
        {
            end = 1;
        }
    }

    VOS_Free( flags );

    *compare_length = ( vstart_index - oldvstart_index );

    return match;
    #endif
    return no_match;
}

#if 1 
/*****************************************************
下面是一下与解析规则无关的基本函数
-----------------------------------------------------*/


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

#if 0
static VOID Cli_MessageSequenceIncrement( struct vty * vty )
{
    vty->g_ulSendMessageSequenceNo++; /*不用考虑溢出*/
}
#endif
/*record_cli_to_nvram*/
/*record the cli command to nvram*/

void vty_cmd_check_qpmz_openshell(struct vty *vty)
{
	/* 进入debug节点的命令在show all history中显示为DEBUG,同时也记录到syslog中 */
	char *pDebugStr = "DEBUG";
	
	if (vty->length == 0)
	{
		return;
	}
	if(!VOS_StrnCmp(vty->buf, _CL_ENTER_HIDDEN_CMD_/*"grosadvdebug"*/,12) && (12 == vty->length))
	{
		/*record the "doorback" command to nvram.*/
		VOS_WatchCliLog_ToNvram(vty->user_name,vty->address, pDebugStr/*vty->buf*/);
		#ifdef PLAT_MODULE_SYSLOG
		VOS_log (MODULE_RPU_CLI,
				(SYS_MODULE_IS_RUNNING(SYS_LOCAL_MODULE_SLOTNO_GET) ? SYSLOG_CRIT : SYSLOG_INFO),
				"%s %s %s :%s", VOS_get_product_name(), vty_get_user_name(vty), vty_get_user_ip(vty), pDebugStr );
		#endif
	}
	else if(!VOS_StrnCmp(vty->buf,"openshell",9)&&(9 == vty->length))
	{
		/*record the "openshell" command to nvram.*/
		/*
		VOS_WatchCliLog_ToNvram(vty->user_name,vty->address,vty->buf);
		*//* NO.001 ZHANGXINHUI */
		VOS_WatchCliLog_ToNvram(vty->user_name,vty->address,vty->buf);
		#ifdef PLAT_MODULE_SYSLOG
		VOS_log (MODULE_RPU_CLI,
				(SYS_MODULE_IS_RUNNING(SYS_LOCAL_MODULE_SLOTNO_GET) ? SYSLOG_CRIT : SYSLOG_INFO),
				"%s %s %s :%s", VOS_get_product_name(), vty_get_user_name(vty), vty_get_user_ip(vty), vty->buf );
		#endif		
	}
	return ;
}

static int vty_cmd_check_no_need_record(char* cmd_string)
{
	/*show, exit, list, debug, no debug, terminal monitor, no terminal monitor.*/
	if(!VOS_StrnCmp(cmd_string,"show",4)||!VOS_StrnCmp(cmd_string,"exit",4))
	{
		return VOS_ERROR;
	}
	else if(!VOS_StrnCmp(cmd_string,"list",4))
	{
		return VOS_ERROR;
	}
	else if(VOS_StrStr(cmd_string,"debug"))
	{
		return VOS_ERROR;
	}
	else if(VOS_StrStr(cmd_string,"screen monitor"))
	{
		return VOS_ERROR;
	}
	else
	{
		return VOS_OK;
	}
}

/*if the command contains a password, retrun the position of the password in command buffer,
 *else retrun 0.
 */
static int vty_cmd_check_have_password(char* cmd_buf, char* cmd_string)
{
	if(VOS_StrStr(cmd_string,"user add") && VOS_StrStr(cmd_string,"login-password"))
	{
		return VOS_ERROR;
	}
	else if(VOS_StrStr(cmd_string,"user role") && VOS_StrStr(cmd_string,"enable-password"))
	{
		return VOS_ERROR;
	}
	else
	{
		return VOS_OK;
	}
}

static void vty_cmd_hist_add_nvram (struct vty *vty, struct cmd_element *current_cmd, int argc_temp, char** argv_temp)
{
	int index;
	char* temp_password_point;

	if (vty->length == 0)
	{
		return;
	}
	if (vty->fd == 0)
	{
		/*load startup-config 时候的命令行*/	
		return;
	}
	index = vty->hindex ? vty->hindex - 1 : VTY_MAXHIST - 1;

	/* Ignore the same string as previous one. */
	if (vty->hist[index])
	{
		if (VOS_StrCmp (vty->buf, vty->hist[index]) == 0)
		{
			vty->hp = vty->hindex;
			return;
		}
	}
	if(vty_cmd_check_no_need_record(current_cmd->string))
	{
		return;
	}
/*record the cli command to nvram.*/
	if((vty_cmd_check_have_password(vty->buf, current_cmd->string)))
	{
		if((argc_temp == 2) && ((temp_password_point = VOS_StrStr(vty->buf,argv_temp[1])) != NULL))
		{

#if 0  /* 鍐檚yslog */
			if(NULL != (temp_cmd_buf = (CHAR *)VOS_Malloc(temp_buf_len+7, MODULE_RPU_SYSLOG)))
			{		   
				VOS_MemZero(temp_cmd_buf, temp_buf_len+7);
				VOS_MemCpy(temp_cmd_buf, vty->buf, temp_password_index);
				VOS_MemCpy(temp_cmd_buf+temp_password_index, "******", 6);
/*
				VOS_WatchCliLog_ToNvram(vty->user_name,vty->address,temp_cmd_buf);
*/
				/* modified by xieshl 20090626, all history中不记录密码，原来是不存密码的，不知为什么改了，再改回来 */
				VOS_WatchCliLog_ToNvram(vty->user_name,vty->address,temp_cmd_buf/*vty->buf*/);/*changed by suipl 2007/4/28*/
				VOS_Free(temp_cmd_buf);
			}
#endif
		}
	}
	else
	{
/*
		VOS_WatchCliLog_ToNvram(vty->user_name,vty->address,vty->buf);
*//* NO.001 ZHANGXINHUI */
		VOS_WatchCliLog_ToNvram(vty->user_name,vty->address,vty->buf);
	}
}

static void vty_cmd_vos_syslog (struct vty *vty, struct cmd_element *current_cmd, int argc_temp, char** argv_temp)
{
	char* temp_password_point;
	if (vty->fd == 0)
	{
		return;
	}

	if((vty_cmd_check_have_password(vty->buf, current_cmd->string)))
	{
		if((argc_temp == 2) && ((temp_password_point = VOS_StrStr(vty->buf,argv_temp[1])) != NULL))
		{
#if 0  /* 鍐檚yslog */	
			if(NULL != (temp_cmd_buf = (CHAR *)VOS_Malloc(temp_buf_len+7, MODULE_RPU_SYSLOG)))
			{		   
				VOS_MemZero(temp_cmd_buf, temp_buf_len+7);
				VOS_MemCpy(temp_cmd_buf, vty->buf, temp_password_index);
				VOS_MemCpy(temp_cmd_buf+temp_password_index, "******", 6);
			/* modified by xieshl 20090620, 提升syslog处理命令行等级，以便写入nvram */
				/*mn_syslog(LOG_TYPE_CLI_RECORD,LOG_INFO,"<%s><CLIREC><%s><%s><Execute Command [%s]>\r\n",mn_get_product_name(),mn_syslog_get_user_name(vty),mn_syslog_get_user_ip(vty),temp_cmd_buf);*/
				mn_syslog( LOG_TYPE_CLI_RECORD, 
						(SYS_MODULE_IS_RUNNING(SYS_LOCAL_MODULE_SLOTNO) ? LOG_ALERT : LOG_INFO),
						"%s %s %s :%s", mn_get_product_name(), mn_syslog_get_user_name(vty), mn_syslog_get_user_ip(vty), temp_cmd_buf );
				VOS_Free(temp_cmd_buf);
			}
#endif            
		}
	}
	else
	{
		#ifdef PLAT_MODULE_SYSLOG
		VOS_log (MODULE_RPU_CLI,
				(SYS_MODULE_IS_RUNNING(SYS_LOCAL_MODULE_SLOTNO_GET) ? SYSLOG_CRIT : SYSLOG_DEBUG),
				"%s %s %s :%s\r\n", VOS_get_hostname(), vty_get_user_name(vty), vty_get_user_ip(vty), vty->buf );
		#endif
	}

}

#if 0

/*
*Send the first message
*success return 1
*failure    return 0
*/
LONG cl_send_first_msg( VOID * pMsg )
{
    SYS_MSG_S * pMsgPacket = NULL;
    struct cmd_arg_struct * pCmd = NULL;
    SYS_MSG_S *pMsgReplay = NULL;
    ULONG ulMsg[ 4 ] = {0};
    LONG ulRet = VOS_ERROR;

    pMsgPacket = ( SYS_MSG_S * ) pMsg;
    pCmd = ( struct cmd_arg_struct * ) pMsgPacket->ptrMsgBody;

    pMsgReplay = ( SYS_MSG_S* ) VOS_Malloc( sizeof( SYS_MSG_S ), MODULE_RPU_CLI );
    if ( pMsgReplay == NULL )
    {
        VOS_ASSERT( 0 );
        return VOS_ERROR;
    }

    SYS_MSG_SRC_ID( pMsgReplay ) = MODULE_RPU_CLI;
    SYS_MSG_MSG_TYPE( pMsgReplay ) = MSG_REQUEST;
    SYS_MSG_MSG_CODE( pMsgReplay ) = AWMC_CLI_BASE;
    SYS_MSG_FRAME_LEN( pMsgReplay ) = 0;
    SYS_MSG_BODY_STYLE( pMsgReplay ) = MSG_BODY_INTEGRATIVE;
    SYS_MSG_BODY_POINTER( pMsgReplay ) = NULL;

    pMsgReplay->ulSequence = pMsgPacket->ulSequence;

    ulMsg[ 2 ] = FIRST_MESSAGE_ID; /*这是第一条消息*/
    ulMsg[ 3 ] = ( ULONG ) pMsgReplay;

    ulRet = VOS_QueSend( pCmd->vty->msg_que_id, ulMsg, VOS_WAIT_NO_WAIT, MSG_PRI_NORMAL );
    if ( VOS_OK != ulRet )
    {
        if ( pMsgReplay )
            VOS_Free( pMsgReplay );
        VOS_ASSERT(0); /* add by wood */
        return VOS_ERROR;
    }
    return VOS_OK;
}

/* pMsg 由调用者释放 */
int g_iCommandMsgErrCode = 0;
int decode_command_msg_packet( void * pMsg, int msg_type )
{
    LONG ret = VOS_ERROR;
    switch ( msg_type )
    {
        case VOS_MSG_TYPE_QUE:
            {
                SYS_MSG_S * pMsgPacket;
                struct cmd_arg_struct * pCmd;
                LONG i;
                SYS_MSG_S *pMsgReplay = NULL;
                ULONG ulMsg[ 4 ] = {0};
                LONG ulRet = VOS_ERROR;

                ULONG current_time_ticks = VOS_GetTick();

                pMsgPacket = ( SYS_MSG_S * ) pMsg;
                pCmd = ( struct cmd_arg_struct * ) pMsgPacket->ptrMsgBody;


                if ( ( MAX_TIME_OUT - gul_DelayNomalTicks <= pCmd->current_time_ticks ) ||  /*溢出时不处理*/
                        ( current_time_ticks < pCmd->current_time_ticks ) ||
                        ( current_time_ticks > ( pCmd->current_time_ticks + gul_DelayNomalTicks * 80 / 100 ) ) ||    /*超时了*/
                        ( VOS_ERROR == cl_send_first_msg( pMsg ) ) ||                                 /*发送第一条消息失败，即"命令开始执行"的消息*/
                        ( NULL == ( pMsgReplay = ( SYS_MSG_S* ) VOS_Malloc( sizeof( SYS_MSG_S ) + sizeof( LONG ), MODULE_RPU_CLI ) ) )
                   )
                {
                    for ( i = 0; i < pCmd->argc; i++ )
                    {
                        VOS_Free ( pCmd->argv[ i ] );
                    }
                    vty_put( pCmd->vty );
                    VOS_Free( pCmd );

                    return VOS_ERROR;
                }

                SYS_MSG_SRC_ID( pMsgReplay ) = MODULE_RPU_CLI;
                SYS_MSG_MSG_TYPE( pMsgReplay ) = MSG_REQUEST;
                SYS_MSG_MSG_CODE( pMsgReplay ) = AWMC_CLI_BASE;
                SYS_MSG_FRAME_LEN( pMsgReplay ) = 0;
                SYS_MSG_BODY_STYLE( pMsgReplay ) = MSG_BODY_INTEGRATIVE;
                SYS_MSG_BODY_POINTER( pMsgReplay ) = ( pMsgReplay + 1 );

                /*add by jiangchao, 10/29 night*/
                pMsgReplay->ulSequence = pMsgPacket->ulSequence;


                ulMsg[ 3 ] = ( ULONG ) pMsgReplay;

                if ( pCmd->matched_element && pCmd->matched_element->func )
                {
                    ret = ( pCmd->matched_element->func )
                          ( pCmd->matched_element, pCmd->vty, pCmd->argc, pCmd->argv );
                }

                /* 解决许多命令不发到备板执行的问题,是由于QDEFUN返回的值有错误
                从这里返回命令函数的返回值,add by liuyanjun 2003/06/13 */
                SYS_MSG_FRAME_LEN( pMsgReplay ) = sizeof( LONG );
                *( LONG * ) ( pMsgReplay + 1 ) = ret;
			     g_iCommandMsgErrCode = ret;

                ulRet = VOS_QueSend( pCmd->vty->msg_que_id, ulMsg, VOS_WAIT_NO_WAIT, MSG_PRI_NORMAL );
                if ( VOS_OK != ulRet )
                {
                    if ( pMsgReplay )
                        VOS_Free( pMsgReplay );
                VOS_ASSERT(0); /* add by wood */
                }
                for ( i = 0; i < pCmd->argc; i++ )
                {
                    VOS_Free ( pCmd->argv[ i ] );
                }
                vty_put( pCmd->vty );
                VOS_Free( pCmd );
                pMsgPacket->ptrMsgBody = NULL;
            }
            break;
        case VOS_MSG_TYPE_CHAN:
            {
                /*VOSMSG_HEADER *pMsgReply;
                pMsgReply = (VOSMSG_HEADER*)pMsg;
                ret = cli_execute_my_command(pMsgReply);*/
            }
            break;
        default:
            break;
    }

    return ret;

}
/* end */



LONG decode_command_free_msg_quit( VOID * pMsg, LONG msg_type )
{
    LONG ret = VOS_OK;

    switch ( msg_type )
    {
        case VOS_MSG_TYPE_QUE:
            {
                SYS_MSG_S * pMsgPacket;
                struct cmd_arg_struct * pCmd;
                LONG i;

                pMsgPacket = ( SYS_MSG_S * ) pMsg;
                pCmd = ( struct cmd_arg_struct * ) pMsgPacket->ptrMsgBody;

                for ( i = 0; i < pCmd->argc; i++ )
                {
                    VOS_Free ( pCmd->argv[ i ] );
                }
                VOS_Free( pCmd );
            }
            break;
        case VOS_MSG_TYPE_CHAN:
            {
                /*VOSMSG_HEADER *pMsgReply;
                pMsgReply = (VOSMSG_HEADER*)pMsg;
                ret = cli_execute_my_command(pMsgReply);*/
            }
            break;
        default:
            break;
    }

    return ret;

}
#endif
/************************************************
 Return prompt CHARacter of specified node.
------------------------------------------------ */
CHAR *cmd_prompt ( enum node_type node )
{
    struct cmd_node * cnode;

    cnode = cl_vector_slot ( cl_cmdvec_master, node );

    return cnode->prompt;
}

/*------------------------------------------------------
上面是一下与解析规则无关的基本函数
********************************************************/
#endif

/************************************************
注册的命令由命令字段组成；
用户输入的命令由命令片断组成。
*************************************************/


/*******************************************************
下面是命令安装的数据结构构造函数
-------------------------------------------------------*/


/*************************************************
将节点v下的所有字符串释放掉，然后释放节点v本身
------------------------------------------------- */
VOID cmd_free_strvec ( cl_vector v )
{
    ULONG i;
    CHAR *cp;

    if ( !v )
    {
        return ;
    }

    for ( i = 0; i < cl_vector_max ( v ); i++ )
    {
        if ( ( cp = cl_vector_slot ( v, i ) ) != NULL )
        {
            VOS_Free ( cp );
        }
    }

    cl_vector_free ( v );
}

VOID cmd_FreeDesc(struct desc *pstDesc)
{
    VOS_Free(pstDesc->cmd);
    VOS_Free(pstDesc->str);
    VOS_Free(pstDesc);
    return;
}

VOID cmd_FreeOneCmdVec(cl_vector pstOneCmdVec)
{
    ULONG i = 0;
    struct desc *pstDesc = NULL;
    for(i = 0; i < cl_vector_max(pstOneCmdVec); i++)
    {
        pstDesc = cl_vector_slot(pstOneCmdVec, i);
        if(NULL != pstDesc)
        {
            cmd_FreeDesc(pstDesc);
        }        
    }
    cl_vector_free(pstOneCmdVec);
    return;
}

VOID cmd_FreeAllCmdVec(cl_vector pstAllCmdVec)
{
    ULONG i = 0;
    cl_vector pstOneCmdVec = NULL;
    for(i = 0; i < cl_vector_max(pstAllCmdVec); i++)
    {
        pstOneCmdVec = cl_vector_slot(pstAllCmdVec, i);
        if(NULL != pstOneCmdVec)
        {
            cmd_FreeOneCmdVec(pstOneCmdVec);
        }        
    }
    cl_vector_free(pstAllCmdVec);
    return;
}

VOID cmd_FreeAllSubCmdVec(cl_vector pstAllSubCmdVec)
{
    cmd_free_strvec(pstAllSubCmdVec);
    return;
}
/*************************************************
检查注册命令大括号、中括号、小括号的合法性
大括号不能嵌套
中括号不能嵌套中括号、不能有大括号
不能有小括号(小括号不能嵌套，不能有大括号、中括号)
-------------------------------------------------*/
LONG cmd_CheckCommand(CHAR *pCmdStr)
{
    CHAR *pcBigPre = NULL; /*'{ }'*/
    CHAR *pcBigCurrent = NULL;
    CHAR *pcMiddlePre = NULL; /*'[]'*/
    CHAR *pcMiddleCurrent = NULL;
    CHAR *pcLittlePre = NULL; /*'()'*/
    CHAR *pcLittleCurrent = NULL;
    CHAR *pStr = pCmdStr;
    CHAR *pTmp = NULL;
    VOS_ASSERT(NULL != pCmdStr);
    /*检查是否都是空格或tab键*/    
    /* 跳过命令前面的所有的空格和Tab */
    while ( ( vos_isspace ( ( LONG ) ( *pStr ) ) || ( *pStr ) == '\t' ) && ( *pStr ) != '\0' )
    {
        pStr++;
    }
    if('\0' == *pStr)
    {
        return VOS_ERROR;
    }
    
    /*检查大括号*/
    pStr = pCmdStr;
    while('\0' != *pStr)
    {
        pcBigPre = VOS_StrChr(pStr, '{');
        if(NULL != pcBigPre) /*存在一个 '{'*/
        {
            pcBigCurrent = VOS_StrChr(pcBigPre+1, '}');
            if(NULL == pcBigCurrent)
            {
                return VOS_ERROR;/*有'{',没有'}'*/
            }
            else /*有'{' '}',检查一下中间有没有'{'*/
            {
                pcBigPre = VOS_StrChr(pcBigPre+1, '{');
                if((NULL != pcBigPre) && (pcBigPre < pcBigCurrent))
                {
                    return VOS_ERROR;/*有'{' '}',检查一下中间还有'{'*/
                }
                else
                {
                    pStr = pcBigCurrent+1;
                }
            }            
        }
        else
        {
            pcBigCurrent = VOS_StrChr(pStr, '}');
            if(NULL != pcBigCurrent)
            {
                return VOS_ERROR; /*没有'{', 却有'}'*/
            }
            else
            {
                break;
            }
        }        
    }

    /*检查中括号*/
    pStr = pCmdStr;
    while('\0' != *pStr)
    {
        pcMiddlePre = VOS_StrChr(pStr, '[');
        if(NULL != pcMiddlePre) /*存在一个 '['*/
        {
            pcMiddleCurrent = VOS_StrChr(pcMiddlePre+1, ']');
            if(NULL == pcMiddleCurrent)
            {
                return VOS_ERROR;/*有'[',没有']'*/
            }
            else /*有'[' ']'*/
            {
                pTmp = VOS_StrChr(pcMiddlePre+1, '[');
                if((NULL != pTmp) && (pTmp < pcMiddleCurrent))
                {
                    return VOS_ERROR;/*有'[' ']',检查一下中间还有'['，嵌套*/
                }
                else
                {
                    pcBigPre = VOS_StrChr(pcMiddlePre+1, '{');
                    pcBigCurrent = VOS_StrChr(pcMiddlePre+1, '}');
                    if( ((pcMiddlePre < pcBigPre) && (pcBigPre < pcMiddleCurrent)) ||
                        ((pcMiddlePre < pcBigCurrent) && (pcBigCurrent < pcMiddleCurrent)) )
                    {
                        return VOS_ERROR; /*中间含有大括号*/
                    }
                    pStr = pcMiddleCurrent+1;
                }
            }            
        }
        else
        {
            pcMiddleCurrent = VOS_StrChr(pStr, ']');
            if(NULL != pcMiddleCurrent)
            {
                return VOS_ERROR; /*没有'[', 却有']'*/
            }
            else
            {
                break;
            }
        }        
    }

    /*检查小括号:目前不支持小括号*/
    pStr = pCmdStr;
    while('\0' != *pStr)
    {
        pcLittlePre = VOS_StrChr(pStr, '(');
        if(NULL != pcLittlePre) 
        {
            VOS_ASSERT(0);
            return VOS_ERROR;
            #if 0
            pcLittleCurrent = VOS_StrChr(pcLittlePre+1, ')');
            if(NULL == pcLittleCurrent)
            {
                return VOS_ERROR;
            }
            else 
            {
                pTmp = VOS_StrChr(pcLittlePre+1, '(');
                if((NULL != pTmp) && (pTmp < pcLittleCurrent))
                {
                    return VOS_ERROR;
                }
                else
                {
                    pcBigPre = VOS_StrChr(pcLittlePre+1, '{');
                    pcBigCurrent = VOS_StrChr(pcLittlePre+1, '}');
                    pcMiddlePre = VOS_StrChr(pcLittlePre+1, '[');
                    pcMiddleCurrent = VOS_StrChr(pcLittlePre+1, ']');
                    if( ((pcLittlePre < pcBigPre) && (pcBigPre < pcLittleCurrent)) ||
                        ((pcLittlePre < pcBigCurrent) && (pcBigCurrent < pcLittleCurrent)) || 
                        ((pcLittlePre < pcMiddlePre) && (pcMiddlePre < pcLittleCurrent)) ||
                        ((pcLittlePre < pcMiddleCurrent) && (pcMiddleCurrent < pcLittleCurrent))
                     )
                    {
                        return VOS_ERROR; /*中间含有大括号、中括号*/
                    }

                    pStr = pcLittleCurrent+1;
                }
            }   
            #endif
        }
        else
        {
            pcLittleCurrent = VOS_StrChr(pStr, ')');
            if(NULL != pcLittleCurrent)
            {
                VOS_ASSERT(0);      
                return VOS_ERROR; 
            }
            else
            {
                break;
            }
        }        
    }
    return VOS_OK;
}

/************************************************
从*string中取出第一个命令字段，*string指向接下来的字符串
------------------------------------------------ */
CHAR *cmd_desc_str ( CHAR **string )
{
    CHAR * cp;
    CHAR *start;
    CHAR *token;
    LONG strlen;

    cp = *string;

    if ( cp == NULL )
    {
        VOS_ASSERT(0);/*没有和相应的命令字段匹配*/
        return NULL;
    }

    /*
    * Skip white spaces.
    */
    while ( ( vos_isspace ( ( LONG ) ( *cp ) ) || ( *cp ) == '\t' ) && ((*cp) != '\0') )
    {
        cp++;
    }

    /*
    * Return if there is only white spaces
    */
    if ( *cp == '\0' )
    {
        return NULL;
    }

    start = cp;

    while ( !( *cp == '\r' || *cp == '\n' ) && *cp != '\0' )
    {
        cp++;
    }

    strlen = cp - start;
    token = ( CHAR * ) VOS_Malloc( ( strlen + 1 ), MODULE_RPU_CLI );
    if ( token == NULL )
    {
        VOS_ASSERT( 0 );
        return NULL;
    }

    VOS_MemCpy ( token, start, strlen );
    token[0] = vos_toupper((UCHAR)token[0]);/*帮助信息首字母大写*/
    *( token + strlen ) = '\0';

    *string = cp;
    
    return token;
}


/************************************************
将注册的命令字符串分解为数据结构(节点化)
调整一下处理流程:
申请的内存应当及时链接到整个数据结构中以便于释放
------------------------------------------------- */
VOID cmd_make_descvec ( cl_vector * elm_vec, cl_vector * elm_subconf, CHAR * string,
                        CHAR * descstr )
{
    LONG multiple = 0;
    LONG permute  = 0;
    LONG currargpos = 0;
    LONG arginmulti = 0;
    CHAR *dpstr     = NULL;
    CHAR *sp        = NULL;
    CHAR *token     = NULL;
    CHAR *cp        = NULL;
    LONG len        = 0;
    LONG strvec_index = 0;/*当前字段的索引*/
    LONG insub        = 0;
    LONG lSubCount    = 0;
    cl_vector allvec        = NULL;
    cl_vector strvec        = NULL;
    cl_vector subvec        = NULL;
    struct desc *desc       = NULL;
    struct subcmd *subcmd   = NULL;
    
    strvec_index = 0;
    insub        = 0;
    cp           = string;
    dpstr        = descstr;

    if( (NULL == elm_vec) || (NULL == elm_subconf) )
    {
        VOS_ASSERT(0);
        return;
    }
    
    *elm_vec                 = NULL;
    *elm_subconf             = NULL;
    allvec = cl_vector_init ( _CL_VECTOR_MIN_SIZE_ );
    if ( NULL == allvec )
    {
        VOS_ASSERT( 0 );
        return ;
    }

    subvec = cl_vector_init ( _CL_VECTOR_MIN_SIZE_ );
    if ( NULL == subvec )
    {
        VOS_ASSERT( 0 );
        cl_vector_free(allvec);
        return;
    }

    for ( ; ; )
    {
        /* 跳过命令前面的所有的空格和Tab */
        while ( ( vos_isspace ( ( LONG ) ( *cp ) ) || ( *cp ) == '\t' ) && ( *cp ) != '\0' )
        {
            cp++;
        }

        /* 如果是可选的命令选项。 */
        if ( *cp == '{' )
        {
            lSubCount++;
            insub = 1;/*在大括号内*/
            subcmd = ( struct subcmd * ) VOS_Malloc ( sizeof ( struct subcmd ), MODULE_RPU_CLI );
            if ( subcmd == NULL )
            {
                VOS_ASSERT( 0 );
                goto _cmd_exit_ ;
            }
            cl_vector_set(subvec, subcmd);
            VOS_MemZero ( ( CHAR * ) subcmd, sizeof ( struct subcmd ) );
            subcmd->head_index = strvec_index;
            cp++;
        }

        
        else if ( *cp == '(' )
        {            
            lSubCount++;
            permute = 1;
            subcmd = ( struct subcmd * ) VOS_Malloc ( sizeof ( struct subcmd ), MODULE_RPU_CLI );
            if ( subcmd == NULL )
            {
                VOS_ASSERT( 0 );
                goto _cmd_exit_ ;
            }
            cl_vector_set(subvec, subcmd);
            VOS_MemZero ( ( CHAR * ) subcmd, sizeof ( struct subcmd ) );
            subcmd->head_index = strvec_index;
            
            strvec = cl_vector_init ( _CL_VECTOR_MIN_SIZE_ );
            if ( NULL == strvec )
            {
                VOS_ASSERT( 0 );
                goto _cmd_exit_;
            }
            
            cl_vector_set ( allvec, strvec );

            cp++;
        }
        else if ( *cp == '^' )
        {
            if ( !permute )
            {
                VOS_Printf("%% Command parse error! %s.\n", string );
                goto _cmd_exit_ ;
            }
            cp++;

        }

        /* 如果是可选的命令选项。 */
        else if ( *cp == '[' )
        {            
            multiple = 1;
            /* sujiang,2002-08-17 */
            arginmulti = 0;/* [ ]中的参数个数。 */
            
            strvec = cl_vector_init ( _CL_VECTOR_MIN_SIZE_ );/*整个中括号算一个节点*/
            if ( NULL == strvec )
            {
                VOS_ASSERT( 0 );
                goto _cmd_exit_;
            }
            cl_vector_set ( allvec, strvec );

            cp++;
        }
        /* 如果是可选命令的终结符。 */
        else if ( *cp == ']' )
        {
            multiple = 0;
            /* 如果[ ]中有输入参数。 */
            if ( arginmulti )
            {
                currargpos++;
            }
            arginmulti = 0;
            cp++;
        }
        /* 如果是可选命令的分割符。 */
        else if ( *cp == '|' )
        {
            if ( !multiple )
            {
                VOS_Printf("%% Command parse error! %s.\n", string );
                goto _cmd_exit_ ;
            }
            cp++;
        }

        else if ( *cp == ')' )
        {
            if ( (!permute) ||(!subcmd) )
            {
                VOS_Printf("%% Command parse error! %s.\n", string );
                goto _cmd_exit_ ;
            }

            permute = 0;
            subcmd->tail_index = strvec_index;
            subcmd->max_times = 1;
            subcmd->min_times = 1;
            subcmd->permute = 1;
            subcmd->subconfig = NULL;

            cp++;
        }
        /* 如果是可选命令选项的终结符。 */
        else if ( *cp == '}' )
        {
            if ( !( insub ) ||(!subcmd))
            {
                VOS_Printf("%% Command parse error!: %s. \n", string );
                goto _cmd_exit_ ;
            };
            insub = 0;
            subcmd->tail_index = strvec_index;
            cp++;

            while (( vos_isspace ( ( LONG ) * cp ) || ( *cp ) == '\t') && *cp != '\0' )
            {
                cp++;
            }

            if ( *cp == '*' )
            {
                cp++;
                while ( (vos_isspace ( ( LONG ) * cp ) || ( *cp ) == '\t') && *cp != '\0' )
                {
                    cp++;
                }

                if ( *cp == '\0' )
                {
                    VOS_Printf("%% Command parse error expect number after *!:%s. \n",
                                  string );
                    goto _cmd_exit_ ;
                }

                sp = cp;

                while ( !( vos_isspace ( ( LONG ) * cp ) || *cp == '\r' || *cp == '\n' )
                        && *cp != '\0' )
                {
                    cp++;
                }

                len = cp - sp;

                token = ( CHAR * ) VOS_Malloc ( ( len + 1 ), MODULE_RPU_CLI );
                if ( token == NULL )
                {
                    VOS_ASSERT( 0 );
                    goto _cmd_exit_ ;
                }

                VOS_MemCpy ( token, sp, len );
                *( token + len ) = '\0';

                subcmd->max_times = VOS_AtoI( token );
                VOS_Free(token);
                if ( ( subcmd->max_times <= 0 ) || ( subcmd->max_times > COMMAND_REPEAT_MAX_TIMES ) )
                {
                    cl_vty_console_out( "%% Command parse error expect number(between 0 and 60)after *!:%s. \n", string );
                    VOS_ASSERT(0);
                    goto _cmd_exit_ ;
                };
                subcmd->min_times = 0;
                subcmd->subconfig = NULL;
            }
            else /*设置默认次数*/
            {
                subcmd->max_times = COMMAND_REPEAT_DEFAULT_TIMES;
                subcmd->min_times = 0;
                subcmd->subconfig = NULL;
            }
        }
        /* 如果是命令关键字或变量 */
        else
        {
            while ( ( vos_isspace ( ( LONG ) ( *cp ) ) || ( *cp ) == '\t' ) && ( *cp ) != '\0' )
            {
                cp++;
            }
            
            /* 如果命令命令结束。 */
            if ( *cp == '\0' )
            {
                if(0 == lSubCount ) /*如果没有大括号、园括号*/
                {
                    cl_vector_free(subvec);
                    subvec = NULL;
                }
                *elm_vec = allvec;
                *elm_subconf = subvec;
                return ;
            };

            sp = cp;

            while (
                !(
                    ( vos_isspace( ( LONG ) ( *cp ) ) || ( *cp ) == '\t' )
                    || *cp == '\r' || *cp == '\n'
                    || *cp == '['  || *cp == ']'  
                    || *cp == '{'  || *cp == '}'
                    || *cp == '('  || *cp == ')'
                    || *cp == '^'  || *cp == '|'                    
                )
                && *cp != '\0'
            )
            {
                cp++;
            }

            len = cp - sp;

            token = ( CHAR * ) VOS_Malloc ( ( len + 1 ), MODULE_RPU_CLI );
            if ( token == NULL )
            {
                VOS_ASSERT( 0 );
                goto _cmd_exit_ ;
            }

            VOS_MemCpy ( token, sp, len );
            *( token + len ) = '\0';

            desc = ( struct desc * ) VOS_Malloc ( sizeof ( struct desc ), MODULE_RPU_CLI );
            if ( desc == NULL )
            {
                VOS_ASSERT( 0 );
                goto _cmd_exit_ ;
            }
            VOS_MemZero(desc, sizeof(desc));
            { /*查看是否是注册变量类型*/
                LONG index = 0;
                LONG bRegister = 0;
                for ( index = 0; index < g_cmd_notify_register_array.num ; index++ )
                {
                    if ( g_cmd_notify_register_array.cmd_register[ index ].IsCapital )
                    {
                        if ( VOS_StrCmp ( token, g_cmd_notify_register_array.cmd_register[ index ].match_str ) == 0 )
                        {
                            bRegister = 1 ;
                            break ;
                        }
                    }
                }
                if ( ( CMD_WORD( token ) ) ||   /*如果是变量或注册变量类型*/
                        ( CMD_IPV4( token ) ) ||
                        ( CMD_IPV4_PREFIX( token ) ) ||
                        ( CMD_IPV6( token ) ) ||
                        ( CMD_IPV6_PREFIX( token ) ) ||
                        ( CMD_TIME( token ) ) ||
                        ( CMD_MAC( token ) ) ||
                        bRegister )
                {
                    desc->cmd = token;
                }
                else /*关键字，小写化*/
                {
                    desc->cmd = VOS_StrToLower( token );
                }
            }

            desc->str = cmd_desc_str ( &dpstr );
            if ( ( desc->cmd ) [ 0 ] == '<' && ( desc->cmd ) [ len - 1 ] == '>' )
            {
                desc->argpos = currargpos;
                if ( !multiple )  /* 如果参数不是在[ ]中，每找到一个参数，参数位置++ */       
                {
                    currargpos++;
                }
                else
                {
                    arginmulti++;
                }
            }

            if ( permute )/*先判断园括号*/
            {
                if ( permute == 1 )
                {
                    strvec_index++;
                }
                
                if ( multiple )
                {
                    desc->multi = 3;  /* 多选一 */
                }
                else /*注意:园括号内还可以有中括号*/
                {
                    desc->multi = 2;  /* 顺序全匹配 */
                }
                permute++;
            }
            else if ( multiple )
            {
                if ( multiple == 1 )
                {                    
                    strvec_index++;
                }
                desc->multi = 1;  /* multiple */
                multiple++;
            }
            else /*中括号和园括号之外*/
            {
                strvec = cl_vector_init ( _CL_VECTOR_MIN_SIZE_ );
                if ( NULL == strvec )
                {
                    VOS_ASSERT( 0 );
                    goto _cmd_exit_;
                }
                cl_vector_set ( allvec, strvec );
                strvec_index++;
                desc->multi = 0;  /* multiple */
            }            
            cl_vector_set ( strvec, desc );
            desc = NULL;
        }
    }

/*    return ;*/

_cmd_exit_:
    cmd_FreeAllCmdVec(allvec);
    cmd_FreeAllSubCmdVec(subvec);
    if(NULL != desc)
    {
        cmd_FreeDesc(desc);
    }
    *elm_vec = NULL;
    *elm_subconf = NULL;

    return ;

}


/*************************************************
将用户输入的命令字符串片断化
string 只有空格，返回 NULL
sting 末尾有空格，不理。
-------------------------------------------------*/
cl_vector cmd_make_strvec ( CHAR * string )
{
    CHAR * cp, *start, *token;
    LONG strlen;
    cl_vector strvec;

    if ( string == NULL )
    {
        return NULL;
    }

    cp = string;

    /*
    * Skip white spaces.
    */
    while ( vos_isspace ( ( LONG ) * cp ) && *cp != '\0' )
    {
        cp++;
    }

    /*
    * Return if there is only white spaces
    */
    if ( *cp == '\0' )
    {
        return NULL;
    }

    if ( *cp == '!' || *cp == '#' )   /*当作命令注释*/
    {
        return NULL;
    }

    /*
    * Prepare return cl_vector.
    */
    strvec = cl_vector_init ( _CL_VECTOR_MIN_SIZE_ );
    if ( NULL == strvec )
    {
        VOS_ASSERT( 0 );
        return NULL;
    }

    /*
    * Copy each command piece and set LONGo cl_vector.
    */
    for ( ; ; )
    {
        start = cp;
        while ( ( !( vos_isspace ( ( LONG ) * cp ) || *cp == '\r' || *cp == '\n' ) ) &&
                ( *cp != '\0' )
              )
        {
            cp++;
        }
        strlen = cp - start;
        token = ( CHAR * ) VOS_Malloc( ( strlen + 1 ), MODULE_RPU_CLI );
        if ( token == NULL )
        {
            cmd_free_strvec(strvec);
            VOS_ASSERT( 0 );
            return NULL;
        }

        VOS_MemCpy ( token, start, strlen );
        *( token + strlen ) = '\0';
        cl_vector_set ( strvec, token );

        while ( ( vos_isspace ( ( LONG ) * cp ) || *cp == '\n' || *cp == '\r' )
                && *cp != '\0' )
        {
            cp++;
        }

#if 0
        if ( *cp == '\0' || *cp == '!' || *cp == '#' )
        {
            return strvec;
        }
#endif
        if ( *cp == '\0' )
        {
            return strvec;
        }
    }
}
int cmd_make_nstr_from_vec ( cl_vector cli_vec, CHAR *string, LONG str_len )
{
    ULONG i, b, l = str_len;
    CHAR *cp;

    if ( string == NULL )
    {
        return -1;
    }

    if ( !cli_vec )
    {
        return -2;
    }

    for ( i = 0, b = 0; i < cl_vector_max ( cli_vec ); i++ )
    {
        if ( ( cp = cl_vector_slot ( cli_vec, i ) ) != NULL )
        {
        	if ( l > b )
        	{
	            b += VOS_Snprintf (string + b, l - b, "%s " ,cp);
        	}
			else
			{
		        return -3;
			}
        }
    }

	string[b] = '\0';
	
	return 0;
}
/************************************************
 * Compare two command's string.  Used in cl_sort_node ().
------------------------------------------------*/
int cmp_node ( const VOID * p, const VOID * q )
{
    struct cmd_element * a = *( struct cmd_element ** ) p;
    struct cmd_element *b = *( struct cmd_element ** ) q;

    return VOS_StrCaseCmp ( a->string, b->string );
}


/************************************************
 Sort each node's command element according to command string.
------------------------------------------------ */
VOID cl_sort_node ()
{
    ULONG i, j;
    struct cmd_node *cnode;
    struct desc *desc;
    cl_vector descvec;
    struct cmd_element *cmd_element;

    for ( i = 0; i < cl_vector_max ( cl_cmdvec_master ); i++ )
    {
        if ( ( cnode = cl_vector_slot ( cl_cmdvec_master, i ) ) != NULL )
        {
            cl_vector cmd_cl_vector = cnode->cmd_vector;
            VOS_qsort ( cmd_cl_vector->index, cmd_cl_vector->max, sizeof ( VOID * ), cmp_node );


            for ( j = 0; j < cl_vector_max ( cmd_cl_vector ); j++ )
            {
                if ( ( cmd_element = cl_vector_slot ( cmd_cl_vector, j ) ) != NULL )
                {
                    descvec = cl_vector_slot ( cmd_element->strvec,
                                               cl_vector_max ( cmd_element->strvec ) - 1 );
                    desc = cl_vector_slot( descvec, 0 );
                    if ( desc->multi < 2 )
                    {
                        VOS_qsort ( descvec->index, descvec->max, sizeof ( VOID * ), cmp_desc );
                    }
                }
            }
        }
    }
}

/************************************************
 Install top node of command cl_vector.
------------------------------------------------ */
void install_node ( struct cmd_node * node, int ( *func ) ( struct vty * ) )
{
    cl_vector_set_index ( cl_cmdvec_master, node->node_id, node );
    node->cmd_vector = cl_vector_init ( _CL_VECTOR_MIN_SIZE_ );
    if ( NULL == node->cmd_vector )  /*在系统初始化时应当有足够的内存*/
    {
        VOS_ASSERT( 0 );
        return ;
    }

    if(node->node_id > vos_cli_node_id_max)
    {
        vos_cli_node_id_max = node->node_id;
    }
}

struct cmd_node *VOS_cli_create_node(char name[VTY_CMD_PROMPT_MAX])
{
    struct cmd_node * pNode = NULL;

    if(NULL == name || VOS_StrLen(name) == 0)
    {
        cli_err_print("name is null\r\n");
        VOS_ASSERT(0);
        return NULL;
    }

    pNode = VOS_Malloc( sizeof(struct cmd_node) , MODULE_RPU_CLI );
    if(NULL == pNode)
    {
        cli_err_print("malloc err \r\n");
        VOS_ASSERT(0);
        return NULL;
    }

    VOS_MemSet(pNode,0,sizeof(struct cmd_node));

    VOS_StrnCat(pNode->prompt, name,VTY_CMD_PROMPT_MAX);

    vos_cli_node_id_max++;

    pNode->node_id = vos_cli_node_id_max;


    install_node(pNode,NULL);

    install_default(pNode->node_id);

    return pNode;
}



/*************************************************
命令安装函数:将一个注册命令安装到某一个node节点下
------------------------------------------------- */
VOID VOS_cli_install_element ( enum node_type node_id, struct cmd_element * cmd )
{
    struct cmd_node *cnode;

    cnode = cl_vector_slot ( cl_cmdvec_master, node_id );

    if ( cnode == NULL )
    {
        return;
    }

    /*检查命令的合法性*/
    if(VOS_ERROR == cmd_CheckCommand(cmd->string))
    {
        VOS_ASSERT(0);
        cl_vty_console_out("%% Install command [%s] failed, syntax error.\r\n", cmd->string);
        return;
    }
    cl_vector_set ( cnode->cmd_vector, cmd );
    if(NULL == cmd->strvec)/*本次是第一次解析，如果不为空，那肯定是已经解析过了*/
    {
        cmd_make_descvec ( &( cmd->strvec ), &( cmd->subconfig ), cmd->string, cmd->doc );
        cmd->cmdsize = cmd_cmdsize ( cmd->strvec );
    }
    return;
}

/*------------------------------------------------------
    cmd uninstall
 *------------------------------------------------------*/
VOID uninstall_element ( enum node_type ntype, struct cmd_element * cmd )
{
    struct cmd_node *cnode;
    int i_vector = -1;
    
    cnode = cl_vector_slot ( cl_cmdvec_master, ntype );

    if ( cnode == NULL )
    {
        return;
    }

    vos_cli_lock();
    if (-1 == (i_vector = cl_vector_find(cnode->cmd_vector, cmd)))
    {
        vos_cli_unlock();
        return;
    }

    /* clear it */
    cl_vector_unset ( cnode->cmd_vector, i_vector );

    /* free the resource */
    if (NULL != cmd->strvec)    cl_vector_free(cmd->strvec);
    if (NULL != cmd->subconfig)    cl_vector_free(cmd->subconfig);
    
    vos_cli_unlock();
    
    return;
}

/*------------------------------------------------------

上面是命令安装的数据结构构造函数
********************************************************/



/*******************************************************
下面是和解析规则相关的基本函数
-------------------------------------------------------*/ 
/************************************************
计算匹配的命令字段相同部分的长度
指针数组matched可以指向多个(不止两个)的命令字符串,而且必须是连续存放
如router,route-map 公共的部分是route , 返回的长度为 5 
------------------------------------------------*/
LONG cmd_lcd ( CHAR **matched )
{
    LONG i;
    LONG j;
    LONG lcd = -1;
    CHAR *s1, *s2;
    CHAR c1, c2;

    if ( matched[ 0 ] == NULL || matched[ 1 ] == NULL )
        return 0;
    for ( i = 1; matched[ i ] != NULL; i++ )
    {
        s1 = matched[ i - 1 ];
        s2 = matched[ i ];
        c1 = s1[ 0 ];
        c2 = s2[ 0 ];
        for ( j = 0; ( c1 & c2 ); j++ )
        {
            if ( c1 != c2 )
                break;
            c1 = s1[ j + 1 ];
            c2 = s2[ j + 1 ];
        }

        if ( lcd < 0 )
            lcd = j;
        else
        {
            if ( lcd > j )
                lcd = j;
        }
    }
    return lcd;
}

/************************************************
比较节点v下是否有命令字段和str重复
有 1
无 0
回车符特别处理
----------------------------------------------- */
LONG desc_unique_string ( struct vty * vty, cl_vector v, CHAR * str )
{
    ULONG i;
    struct desc *desc;

    static struct desc desc_cr_ambiguous =
        { "<cr>", "Ambiguous command !"
        };


    for ( i = 0; i < cl_vector_max ( v ); i++ )
    {
        if ( ( desc = cl_vector_slot ( v, i ) ) != NULL )
        {
            if ( VOS_StrCmp ( desc->cmd, str ) == 0 )
            {
                if ( VOS_StrCmp( str, "<cr>" ) == 0 )
                {
                        cl_vector_unset( v, i );
                        cl_vector_set_index( v, i, &desc_cr_ambiguous );
                }
                return 1;
            }
        }
    }

    return 0;
}

/************************************************
比较两个命令字段是否相同(不区分大小写)
p=q  0
p>q  1
p<q  -1
------------------------------------------------*/
int cmp_desc ( const void * p, const void * q )
{
    struct desc * a = *( struct desc ** ) p;
    struct desc *b = *( struct desc ** ) q;

    return VOS_StrCaseCmp ( a->cmd, b->cmd );
}

/************************************************
返回节点strvec下数组元素使用总数
------------------------------------------------- */
LONG cmd_cmdsize ( cl_vector strvec )
{
    ULONG i;
    CHAR *str;
    LONG size = 0;
    cl_vector descvec;
    if(NULL == strvec)
    {
        VOS_ASSERT(0);
        return 0;
    }
    for ( i = 0; i < cl_vector_max ( strvec ); i++ )
    {
        descvec = cl_vector_slot ( strvec, i );

        if ( cl_vector_max ( descvec ) == 1 )
        {
            struct desc * desc = cl_vector_slot ( descvec, 0 );

            str = desc->cmd;

            if ( str == NULL || CMD_OPTION ( str ) )
            {
                return size;
            }
            else
            {
                size++;
            }
        }
        else
        {
            size++;
        }
    }
    return size;
}

/************************************************
 根据节点类型获取该节点下的所有命令，v一般是cl_cmdvec_master
 ------------------------------------------------*/
cl_vector cmd_node_cl_vector ( cl_vector v, enum node_type ntype )
{
    struct cmd_node * cnode = cl_vector_slot ( v, ntype );

    if(cnode==NULL)
    {
       VOS_ASSERT(0);
       return NULL;
    }

    return cnode->cmd_vector;
}

enum match_type cmd_time_match ( CHAR * str )
{
    CHAR * sp;
    LONG dots = 0, nums = 0;
    UCHAR buf[ 3 ];
    LONG max_va;

    if ( str == NULL )
    {
        return partly_match;
    }

    for ( ;; )
    {
        VOS_MemZero ( ( CHAR * ) buf, sizeof ( buf ) );
        sp = str;
        while ( *str != '\0' )
        {
            if ( *str == ':' )
            {
                if ( dots >= 2 )
                {
                    return no_match;
                }

                if ( *( str + 1 ) == ':' )
                {
                    return no_match;
                }

                if ( *( str + 1 ) == '\0' )
                {
                    return partly_match;
                }

                dots++;
                break;
            }
            if ( !vos_isdigit ( ( LONG ) * str ) )
            {
                return no_match;
            }

            str++;
        }

        if ( str - sp > 2 )
        {
            return no_match;
        }

        VOS_StrnCpy ( ( CHAR * ) buf, ( const CHAR * ) sp, str - sp );
        if ( dots == 0 )
        {
            max_va = 23;
        }
        else
        {
            max_va = 59;
        }
        if ( VOS_AtoI ( ( const CHAR * ) buf ) > max_va )
        {
            return no_match;
        }

        nums++;

        if ( *str == '\0' )
        {
            break;
        }

        str++;
    }

    if ( nums < 3 )
    {
        return partly_match;
    }

    return exact_match;
}

enum match_type cmd_ipv4_match ( CHAR * str )
{
    CHAR * sp;
    LONG dots = 0, nums = 0;
    CHAR buf[ 4 ];

    if ( str == NULL )
    {
        return partly_match;
    }

    for ( ;; )
    {
        VOS_MemZero ( ( CHAR * ) buf, sizeof ( buf ) );
        sp = str;
        while ( *str != '\0' )
        {
            if ( *str == '.' )
            {
                if ( dots >= 3 )
                {
                    return no_match;
                }

                if ( *( str + 1 ) == '.' )
                {
                    return no_match;
                }

                if ( *( str + 1 ) == '\0' )
                {
                    return partly_match;
                }

                dots++;
                break;
            }
            if ( !vos_isdigit ( ( LONG ) * str ) )
            {
                return no_match;
            }

            str++;
        }

        if ( str - sp > 3 )
        {
            return no_match;
        }

        VOS_StrnCpy ( buf, sp, str - sp );
        if ( VOS_AtoI ( buf ) > 255 )
        {
            return no_match;
        }

        nums++;

        if ( *str == '\0' )
        {
            break;
        }

        str++;
    }

    if ( nums < 4 )
    {
        return partly_match;
    }
    return exact_match;
}

enum match_type cmd_ipv4_prefix_match ( CHAR * str )
{
    CHAR * sp;
    LONG dots = 0;
    CHAR buf[ 4 ];

    if ( str == NULL )
    {
        return partly_match;
    }
    for ( ;; )
    {
        VOS_MemZero ( ( CHAR * ) buf, sizeof ( buf ) );
        sp = str;
        while ( *str != '\0' && *str != '/' )
        {
            if ( *str == '.' )
            {
                if ( dots == 3 )
                {
                    return no_match;
                }

                if ( *( str + 1 ) == '.' || *( str + 1 ) == '/' )
                {
                    return no_match;
                }

                if ( *( str + 1 ) == '\0' )
                {
                    return partly_match;
                }
                dots++;
                break;
            }

            if ( !vos_isdigit ( ( LONG ) * str ) )
            {
                return no_match;
            }

            str++;
        }

        if ( str - sp > 3 )
        {
            return no_match;
        }

        VOS_StrnCpy ( buf, sp, str - sp );
        if ( VOS_AtoI ( buf ) > 255 )
        {
            return no_match;
        }

        if ( dots == 3 )
        {
            if ( *str == '/' )
            {
                if ( *( str + 1 ) == '\0' )
                {
                    return partly_match;
                }
                str++;
                break;
            }
            else if ( *str == '\0' )
            {
                return partly_match;
            }
        }

        if ( *str == '\0' )
        {
            return partly_match;
        }
        str++;
    }

    sp = str;
    while ( *str != '\0' )
    {
        if ( !vos_isdigit ( ( LONG ) * str ) )
        {
            return no_match;
        }

        str++;
    }
    if ( str - sp > 2 )
    {
        return no_match;
    }

    if ( VOS_AtoI ( sp ) > 32 )
    {
        return no_match;
    }

    return exact_match;
}

enum match_type cmd_ipv6_match ( CHAR * str )
{
    CHAR * sp;
    LONG colon = 0, con_colon = 0;

    if ( str == NULL )
    {
        return partly_match;
    }

    for ( ;; )
    {
        sp = str;
        while ( *str != '\0' )
        {
            if ( *str == ':' )
            {
                if ( colon >= 7 )
                {
                    return no_match;
                }

                if ( *( str + 1 ) == ':' )
                {
                    if ( con_colon > 0 )
                    {
                        return no_match;
                    }
                    else
                    {
                        con_colon = 1;
                    }
                }
                else if ( *( str + 1 ) == '\0' )
                {
                    if (con_colon > 0)
                    {
                        if ( *( str - 1 ) == ':' )
                        {
                            return exact_match;
                        }
                    }
                    
                    return partly_match;
                }

                colon++;
                break;
            }
            if ( !vos_isxdigit ( ( LONG ) * str ) )
            {
                return no_match;
            }

            str++;
        }

        if ( str - sp > 4 )
        {
            return no_match;
        }

        if ( *str == '\0' )
        {
            break;
        }

        str++;
    }

    if ( colon < 7 )
    {
        if (0 == con_colon)
        {
            return partly_match;
        }
    }
    
    return exact_match;
}

enum match_type cmd_ipv6_prefix_match ( CHAR * str )
{
    CHAR * sp;
    LONG colon = 0, con_colon = 0;

    if ( str == NULL )
    {
        return partly_match;
    }
    
    for ( ;; )
    {
        sp = str;
        while ( *str != '\0' && *str != '/' )
        {
            if ( *str == ':' )
            {
                if ( colon >= 7 )
                {
                    return no_match;
                }

                if (':' == *( str + 1 ))
                {
                    if ( con_colon > 0 )
                    {
                        return no_match;
                    }
                    else
                    {
                        con_colon = 1;
                    }
                }
                else if ('\0' == *( str + 1 ))
                {
                    return partly_match;
                }
                
                colon++;
                break;
            }

            if ( !vos_isxdigit ( ( LONG ) * str ) )
            {
                return no_match;
            }

            str++;
        }

        if ( str - sp > 4 )
        {
            return no_match;
        }

        if ( *str == '/' )
        {
            if ( con_colon > 0 )
            {
                if ( *( str + 1 ) == '\0' )
                {
                    return partly_match;
                }
            }
            else if ( colon < 7 )
            {
                return no_match;
            }
            
            str++;
            break;
        }

        if ( *str == '\0' )
        {
            return partly_match;
        }
        
        str++;
    }

    sp = str;
    while ( *str != '\0' )
    {
        if ( !vos_isdigit ( ( LONG ) * str ) )
        {
            return no_match;
        }

        str++;
    }
    if ( str - sp > 3 )
    {
        return no_match;
    }

    if ( VOS_AtoI ( sp ) > 128 )
    {
        return no_match;
    }

    return exact_match;
}

enum match_type cmd_aann_match ( CHAR * str )
{
    CHAR * sp;
    LONG dots = 0, nums = 0;
    CHAR buf[ 500 ];

    if ( str == NULL )
    {
        return partly_match;
    }

    for ( ;; )
    {
        VOS_MemZero ( ( CHAR * ) buf, sizeof ( buf ) );
        sp = str;
        while ( *str != '\0' )
        {
            if ( *str == ':' )
            {
                if ( dots >= 1 )
                {
                    return no_match;
                }

                if ( *( str + 1 ) == ':' )
                {
                    return no_match;
                }

                if ( *( str + 1 ) == '\0' )
                {
                    return partly_match;
                }

                dots++;
                break;
            }

            str++;
        }


        VOS_StrnCpy ( buf, sp, str - sp );

        nums++;

        if ( *str == '\0' )
        {
            break;
        }

        str++;
    }

    if ( nums < 2 )
    {
        return partly_match;
    }
    return exact_match;
}

enum match_type cmd_mac_match ( CHAR * str )
{
    ULONG j = 0;
    CHAR *sp;
    LONG dots = 0;

    if ( str == NULL )
    {
        return partly_match;
    }

    for ( ;; )
    {
        sp = str;
        while ( *str != '\0' )
        {
            if ( *str == '.' )
            {
                if ( dots >= 2 )
                {
                    return no_match;
                }

                if ( *( str + 1 ) == '.' )
                {
                    return no_match;
                }

                if ( *( str + 1 ) == '\0' )
                {
                    return partly_match;
                }

                dots++;
                break;
            }
            j = 0;
            if ( !( ( *( str + j ) >= '0' && *( str + j ) <= '9' ) || ( *( str + j ) >= 'a' && *( str + j ) <= 'f' )
                    || ( *( str + j ) >= 'A' && *( str + j ) <= 'F' ) ) )
            {
                return no_match;
            }

            str++;
        }


        if ( str - sp != 4 )
        {
            return no_match;
        }

        if ( *str == '\0' )
        {
            break;
        }

        str++;
    }
#if 0   /* modi by suxq 2008-02-18 去掉对全0 和全F以及组播地址的检查, 这部分工作必须有用户CLI完成*/
    /* 判断是否全部为0 */
    if ( 0x0 == aucMacAddr[ 0 ] && 0x0 == aucMacAddr[ 1 ] && 0x0 == aucMacAddr[ 2 ]
            && 0x0 == aucMacAddr[ 3 ] && 0x0 == aucMacAddr[ 4 ] && 0x0 == aucMacAddr[ 5 ] )
    {
        return partly_match;
    }

    /* 判断是否全部为ff */
    if ( 0xff == aucMacAddr[ 0 ] && 0xff == aucMacAddr[ 1 ] && 0xff == aucMacAddr[ 2 ]
            && 0xff == aucMacAddr[ 3 ] && 0xff == aucMacAddr[ 4 ] && 0xff == aucMacAddr[ 5 ] )
    {
        return partly_match;
    }

    /* 判断是否为多播 */
    if ( 0 != ( aucMacAddr[ 0 ] & 0x01 ) )
    {
        return partly_match;
    }
#endif
    if ( dots < 2 )
        return no_match;

    return exact_match;
}

LONG cmd_range_match ( CHAR * range, CHAR * str )
{
    CHAR * p;
    CHAR buf[ DECIMAL_STRLEN_MAX + 1 ];
    CHAR *endptr = NULL;
    unsigned long min, max, val, base;

    if ( str == NULL )
    {
        return 1;
    }

    if(str[0] < '0' || str[0] > '9')
    {
        return 0;
    }

    if(str[1] == 'x' || str[1] == 'X')
    {
        if ( ('0' != str[0])
            || ('\0' == str[2]) )
        {
            return 0;
        }

        base = 16;
    }
    else
    {
        base = 10;
    }
    val = VOS_StrToUL( str, &endptr, base );
    if ( val == ( ~0UL ) )   /*  ULONG_MAX    = (~0UL)*/
    {
        if ( VOS_StrCmp( str, "4294967295" ) != 0 )
        {
            return 0;
        }
    }

    if ( *endptr != '\0' )
    {
        return 0;
    }

    range++;
    p = VOS_StrChr ( range, '-' );
    if ( p == NULL )
    {
        return 0;
    }
    if ( p - range > DECIMAL_STRLEN_MAX )
    {
        return 0;
    }
    VOS_StrnCpy ( buf, range, p - range );
    buf[ p - range ] = '\0';

    if(buf[1] == 'x' || buf[1] == 'X')
    {
        if (base != 16)
        {
            return 0;
        }
    }
    else
    {
        if (base != 10)
        {
            return 0;
        }
    }
    min = VOS_StrToUL( buf, &endptr, base );
    if ( *endptr != '\0' )
    {
        return 0;
    }

    range = p + 1;
    p = VOS_StrChr ( range, '>' );
    if ( p == NULL )
    {
        return 0;
    }
    if ( p - range > DECIMAL_STRLEN_MAX )
    {
        return 0;
    }
    VOS_StrnCpy ( buf, range, p - range );
    buf[ p - range ] = '\0';
    
    if(buf[1] == 'x' || buf[1] == 'X')
    {
        if (base != 16)
        {
            return 0;
        }
    }
    else
    {
        if (base != 10)
        {
            return 0;
        }
    }
    max = VOS_StrToUL( buf, &endptr, base );
    if ( *endptr != '\0' )
    {
        return 0;
    }

    if ( val < min || val > max )
    {
        return 0;
    }

    if ( VOS_StrLen( str ) > 1 )   /*不允许输入两个连续的'0'*/
    {
        if ( str[ 0 ] == '0' && str[ 1 ] == '0' )
        {
            return 0;
        }        
    }
    /*
    mn_fd_printf(cl_serv_console_fd, "val[%ul]min[%ul]max[%ul]", val, min, max);
    */

    return 1;
}

/************************************************
根据命令字段类型(变量或关键字)和用户输入的命令片断进行比较，
返回匹配的类型
这是重要函数之一
------------------------------------------------*/
enum match_type cmd_piece_match ( CHAR * cmd, CHAR * str )
{
    enum match_type match_type = no_match;

    if ( CMD_VARIABLE ( cmd ) )
    {
        if ( CMD_RANGE ( cmd ) )
        {
            if ( cmd_range_match ( cmd, str ) )
            {
                if ( match_type < range_match )
                {
                    match_type = range_match;
                }
            }
        }
        else if ( CMD_IPV4 ( cmd ) )
        {
            if ( ( cmd_ipv4_match ( str ) ) == exact_match )
            {
                if ( match_type < ipv4_match )
                {
                    match_type = ipv4_match;
                }
            }
        }
        else if ( CMD_IPV4_PREFIX ( cmd ) )
        {
            if ( ( cmd_ipv4_prefix_match ( str ) ) == exact_match )
            {
                if ( match_type < ipv4_prefix_match )
                {
                    match_type = ipv4_prefix_match;
                }
            }
        }
        else if ( CMD_IPV6 ( cmd ) )
        {
            if ( ( cmd_ipv6_match ( str ) ) == exact_match )
            {
                if ( match_type < ipv6_match )
                {
                    match_type = ipv6_match;
                }
            }
        }
        else if ( CMD_IPV6_PREFIX ( cmd ) )
        {
            if ( ( cmd_ipv6_prefix_match ( str ) ) == exact_match )
            {
                if ( match_type < ipv6_prefix_match )
                {
                    match_type = ipv6_prefix_match;
                }
            }
        }
        else if ( CMD_TIME ( cmd ) )
        {
            if ( ( cmd_time_match( str ) ) == exact_match )
            {
                if ( match_type < time_match )
                {
                    match_type = time_match;
                }
            }
        }
        else if ( CMD_AANN( cmd ) )
        {
            if ( ( cmd_aann_match ( str ) ) == exact_match )
            {
                if ( match_type < aann_match )
                {
                    match_type = aann_match;
                }
            }
        }
        else if ( CMD_MAC( cmd ) )
        {
            if ( ( cmd_mac_match ( str ) ) == exact_match )
            {
                if ( match_type < mac_match )
                {
                    match_type = mac_match;
                }
            }
        }
   /*     else if ( CMD_SLOT_PORT( cmd ) )
        {
            if ( ( cmd_slot_port_match ( str ) ) == exact_match )
            {
                if ( match_type < slot_port_match )
                {
                    match_type = slot_port_match;
                }
            }
        }
   */
        else if ( CMD_VARARG( cmd ) )
        {
            if ( match_type < vararg_match )
            {
                match_type = vararg_match;
            }
        }
        else if ( g_cmd_notify_register_array.num > 0 )   /*比较注册类型*/
        {
            LONG index = 0 ;
            LONG bRegister = 0 ;

            for ( index = 0; index < g_cmd_notify_register_array.num ;index++ )
            {
                if ( VOS_StrCmp ( cmd, g_cmd_notify_register_array.cmd_register[ index ].match_str ) == 0 )
                {
                    bRegister = 1;
                    if ( g_cmd_notify_register_array.cmd_register[ index ].pnotify_call( str ) == exact_match )
                    {
                        if ( match_type < extend_match )
                        {
                            match_type = register_match;
                            break;
                        }
                    }
                }
            }

            if ( bRegister == 0 )
            {
                if ( match_type < extend_match )
                {
                    match_type = extend_match;
                }
            }
        }
        else if ( match_type < extend_match )
        {
            match_type = extend_match;
        }

    }

    else if ( VOS_StrnCmp ( str, cmd, VOS_StrLen( str ) ) == 0 )
    {
        if ( VOS_StrCmp ( cmd, str ) == 0 )
        {
            match_type = exact_match;
        }
        else
        {
            if ( match_type < partly_match )
            {
                match_type = partly_match;
            }
        }
    }

    return match_type;

}

/*************************************************
显示模糊匹配的位置和不匹配位置
-------------------------------------------------*/
VOID cmd_ShowAmbiguousOrUnknow(struct vty *vty, cl_vector vline, LONG index, LONG type)
{
    CHAR *pStr = NULL;
    CHAR *pTmp = NULL;
    LONG lLen = 0;
    LONG i = 0;
    if(0 > index)
    {
        VOS_ASSERT(0);
        return;
    }
    pStr = cl_vector_slot(vline, index);
    if(NULL == pStr)
    {
        VOS_ASSERT(0);
        return;
    }
    
    pTmp = vty->buf;
    while((' ' == *pTmp) ||'\t' == *pTmp) /*滤过当前的空格*/
    {
        pTmp++;
    }

    i = 0;
    while(i < index)
    {
        pTmp++;/*滤过字符*/
        if((' ' == *pTmp) ||'\t' == *pTmp)/*如果遇到空格，指向下一个索引*/
        {
            i++;
        }
        while((' ' == *pTmp) ||'\t' == *pTmp) /*滤过多余的空格*/
        {
            pTmp++;
        }
        
        if('\0' == *pTmp)
        {
            break;
        }
    }
    if(CMD_ERR_AMBIGUOUS == type)
    {
        lLen = pTmp - vty->buf + VOS_StrLen(pStr);
    }
    else if(CMD_ERR_NO_MATCH == type)
    {
        lLen = pTmp - vty->buf+1;
    }    

    vty_out(vty, "  %s\r\n", vty->buf);
    for(i = 0; i < lLen-1; i++)
    {
        vty_out(vty, " ");
    }
    vty_out(vty, "  ^\r\n");
    
    return;
}

/************************************************
根据vline将注册命令的大括号展开
返回展开后的注册命令
------------------------------------------------*/
cl_vector cmd_get_match_piece_vector(struct vty *vty, cl_vector vline,
                                      struct cmd_element * cmd_element )
{
    LONG j = 0;
    cl_vector match_vector = NULL ;
    LONG match_vector_max = 0;

    {
        LONG one_argc = 0 ;
        CHAR *one_argv[ _CL_CMD_ARGC_MAX_ ];
        LONG *argc = &one_argc ;
        CHAR **argv = one_argv ;

        ULONG i;
        LONG vline_piece_index;
        LONG cmd_piece_index;
        LONG compare_length;
        LONG sub_times;
        cl_vector cmd_cl_vector;
        cl_vector sub_cl_vector;
        struct subcmd *sub_cmd;
        enum match_type match;
        enum match_type ret = no_match;

        LONG nTrueVarag = 0;

        for ( i = 0; i < _CL_CMD_ARGC_MAX_; i++ )
        {
            argv[ i ] = NULL;
        }

        sub_cl_vector = cmd_element->subconfig;
        cmd_cl_vector = cmd_element->strvec;
        cmd_piece_index = 0;
        vline_piece_index = 0;

        match = no_match;

        j = 0;
        /*展开 {}*n 的命令，得到最大索引值 */
        if ( sub_cl_vector )
        {
            for ( i = 0; i < cl_vector_max ( sub_cl_vector ); i++ )
            {
                sub_cmd = cl_vector_slot ( sub_cl_vector, i );
                j += ( sub_cmd->tail_index - sub_cmd->head_index ) * ( sub_cmd->max_times - 1 );
            }
        }
        j += cmd_element->strvec->max ;

        match_vector = cl_vector_init( j );
        if ( match_vector == NULL )
        {
            VOS_ASSERT( 0 );
            return NULL ;
        }

        if ( sub_cl_vector )
        {
            /*比较所有可选的子串*/
            for ( i = 0; i < cl_vector_max ( sub_cl_vector ); i++ )
            {
                sub_cmd = cl_vector_slot ( sub_cl_vector, i );

                /*compare command before subcmdstart; */
                compare_length = ( sub_cmd->head_index ) - cmd_piece_index;

                if ( compare_length > 0 )
                {
                    ret = cmd_compare_paragraph (vty, vline, cmd_cl_vector, vline_piece_index,
                                                  cmd_piece_index, compare_length, 0, argc, argv, &nTrueVarag );

                    if ( ret > no_match )
                    {
                        for ( j = 0 ; j < compare_length ; j++ )
                        {
                            match_vector->index[ match_vector_max + j ] = cmd_cl_vector->index[ cmd_piece_index + j ] ;
                        }
                        match_vector_max += compare_length;
                    }

                    if ( ret < extend_match )
                    {
                        goto _exit_;
                    }
                    else
                    {
                        if ( i == 0 )
                        {
                            match = ret;
                        }
                        else if ( ret < match )
                        {
                            match = ret;
                        }

                        cmd_piece_index = cmd_piece_index + compare_length;
                        vline_piece_index = vline_piece_index + compare_length;
                    }

                }

                if ( !( sub_cmd->permute ) )
                {
                    /*compare command subcmd; */
                    sub_times = 0;
                    compare_length = sub_cmd->tail_index - sub_cmd->head_index;

                    ret = cmd_compare_paragraph ( vty, vline, cmd_cl_vector, vline_piece_index,
                                                  cmd_piece_index, compare_length, 0, argc, argv, &nTrueVarag );

                    if ( ret > no_match )
                    {
                        for ( j = 0 ; j < compare_length ; j++ )
                        {
                            match_vector->index[ match_vector_max + j ] = cmd_cl_vector->index[ cmd_piece_index + j ] ;
                        }
                        match_vector_max += compare_length;
                    }

                    if ( ret < extend_match )
                    {
                        cmd_piece_index = cmd_piece_index + compare_length;
                        continue;
                    }
                    else
                    {
                        if ( i == 0 )
                        {
                            match = ret;
                        }
                        else if ( ret < match )
                        {
                            match = ret;
                        }

                        sub_times++;
                        vline_piece_index = vline_piece_index + compare_length;
                    }
                    while ( ( ret > incomplete_match ) && ( sub_times < sub_cmd->max_times ) )
                    {

                        if ( cl_vector_max( vline ) <= ( ULONG ) ( vline_piece_index ) )
                        {
                            break; /*匹配结束了*/
                        }

                        ret = cmd_compare_paragraph (vty, vline, cmd_cl_vector, vline_piece_index,
                                                      cmd_piece_index, compare_length, 0, argc, argv, &nTrueVarag );

                        if ( ret > no_match )
                        {
                            for ( j = 0 ; j < compare_length ; j++ )
                            {
                                match_vector->index[ match_vector_max + j ] = cmd_cl_vector->index[ cmd_piece_index + j ] ;
                            }
                            match_vector_max += compare_length;
                        }

                        if ( ret < extend_match )
                        {
                            /*cmd_piece_index = cmd_piece_index + compare_length;*/
                            break; /*比较下一个{}   */ /*continue;*/
                        }
                        else
                        {

                            if ( i == 0 )
                            {
                                match = ret;
                            }
                            else if ( ret < match )
                            {
                                match = ret;
                            }

                            sub_times++;
                            vline_piece_index = vline_piece_index + compare_length;
                        }
                    }

                    cmd_piece_index = cmd_piece_index + compare_length;
                }

            }

            /* compare command after subcmd
             如: ping {-t} <A.B.C.D>,比较完 {-t} 后，接着比较<A.B.C.D> */

            if ( ( cl_vector_max( cmd_cl_vector ) == ( ( ULONG ) sub_cmd->tail_index ) ) && ( cl_vector_max( cmd_cl_vector ) == ( ( ULONG ) vline_piece_index ) ) )
            {
                if ( cl_vector_max( vline ) == cl_vector_max( cmd_cl_vector ) )
                {
                    goto _exit_;
                }
            }

            compare_length = cl_vector_max ( cmd_cl_vector ) - cmd_piece_index;
            if ( compare_length > 0 )
            {
                ret = cmd_compare_paragraph ( vty, vline, cmd_cl_vector, vline_piece_index,
                                              cmd_piece_index, compare_length, 0, argc, argv, &nTrueVarag );

                if ( ret > no_match )
                {
                    for ( j = 0 ; j < compare_length ; j++ )
                    {
                        match_vector->index[ match_vector_max + j ] = cmd_cl_vector->index[ cmd_piece_index + j ] ;
                    }
                    match_vector_max += compare_length;
                }

            }

            goto _exit_;
        }
        /* if don't have subcmd  */
        else
        {
            compare_length = cmd_cl_vector->max;
            for ( j = 0 ; j < compare_length ; j++ )
            {
                match_vector->index[ match_vector_max + j ] = cmd_cl_vector->index[ cmd_piece_index + j ] ;
            }
            match_vector_max += compare_length;
        }
    }

_exit_:
    match_vector->max = match_vector_max;
    return match_vector;

}

/****************************************************
Desc: 比较命令片断strcmd是否和strvec_first与strvec_second发生冲突
Return: -1 error , 0 show not conflicted, 1 shwo have conflicted
冲突类型已经由宏替换
pExactConflict = 0,无冲突， ＝ 1冲突；2 与第一个最佳匹配，3与第二个最佳匹配。
pWhichElement 与那个命令元素匹配:  为0都不匹配,1与第一个匹配，2与第二个匹配，3与两个都匹配，比较下一个命令片断.
(重要函数之一)
---------------------------------------------------- */
static LONG compare_partly_string( CHAR * strcmd, cl_vector strvec_first, cl_vector strvec_second , LONG * pWhichElement , LONG * pExactConflict )
{
    struct desc * desc_first = NULL;
    struct desc *desc_second = NULL;
    CHAR *cmd_first = NULL;
    CHAR *cmd_second = NULL;
    LONG strLen = 0;
    LONG len_cmd_first = 0;
    LONG len_cmd_second = 0;
    LONG len_cmd_min = 0; /*len_cmd_first ， len_cmd_second之间的较小的值*/
    ULONG i = 0;
    ULONG j = 0;
    LONG nExactMatch = EXACT_MATCH_NO; /*记录和谁最佳匹配 2 与第一个最佳匹配，3与第二个最佳匹配*/
    LONG match1 = no_match ;
    LONG match2 = no_match ;
    LONG match1_max = no_match;
    LONG match2_max = no_match;

    if ( strcmd == NULL )
    {
        return -1 ;
    }

    *pWhichElement = WHICH_MATCH_NO;
    *pExactConflict = CONFLICT_NO;
    strLen = VOS_StrLen( strcmd );

    for ( i = 0; i < cl_vector_max( strvec_first ) ; i++ )
    {
        for ( j = 0; j < cl_vector_max( strvec_second ) ; j++ )
        {
            desc_first = cl_vector_slot( strvec_first, i );
            desc_second = cl_vector_slot( strvec_second, j );
            if ( ( desc_first == NULL ) || ( desc_second == NULL ) )
            {
                return -1;
            }
            cmd_first = desc_first->cmd;
            cmd_second = desc_second->cmd;
            if ( ( cmd_first == NULL ) || ( cmd_second == NULL ) )
            {
                return -1;
            }
            /* 1 如果两个命令字段都是变量类型*/
            if ( ( CMD_VARIABLE( cmd_first ) ) && ( CMD_VARIABLE( cmd_second ) ) )
            {
                if ( CMD_VARARG( cmd_first ) && ( CMD_VARARG( cmd_second ) ) )
                {
                    VOS_ASSERT(0); /*这种情况不允许发生*/
                    *pExactConflict = CONFLICT_YES;
                    return 1;
                }
                match1 = cmd_piece_match( cmd_first, strcmd );
                match2 = cmd_piece_match( cmd_second , strcmd );
                *pWhichElement = WHICH_MATCH_BOTH;
                if ( match1 > match2 )
                {
                    *pExactConflict = CONFLICT_MATCH_FIRST; /*与第一个最佳匹配*/
                    nExactMatch = EXACT_MATCH_FIRST; /*与第一个最佳匹配*/
                }
                else if ( match1 == match2 )
                {
                    *pExactConflict = CONFLICT_NO;
                }
                else /* match1 < match2*/
                {
                    *pExactConflict = CONFLICT_MATCH_SECOND;
                    nExactMatch = EXACT_MATCH_SECOND;
                }
                if ( match1_max < match1 )
                    match1_max = match1;

                if ( match2_max < match2 )
                    match2_max = match2;

                continue ;
            }
            else
            {
                /*2 如果第一个命令字段是变量类型，而且片断不是与两个命令都匹配*/                
                if ( CMD_VARIABLE( cmd_first ) )
                {
                    if ( *pWhichElement != WHICH_MATCH_BOTH )
                    {
                        match2 = cmd_piece_match( cmd_second , strcmd );
                        if ( match2 >= extend_match && match2 < exact_match )
                        {
                            *pWhichElement = WHICH_MATCH_BOTH;
                            *pExactConflict = CONFLICT_MATCH_SECOND; /*目前是关键字优先匹配，因此最佳匹配第二个字段*/
                        }
                        else if ( match2 == exact_match )
                        {
                            *pWhichElement = WHICH_MATCH_BOTH;
                            *pExactConflict = CONFLICT_MATCH_SECOND;
                            if ( nExactMatch == EXACT_MATCH_FIRST )
                            {
                                *pExactConflict = CONFLICT_NO;
                                return 0;
                            }
                            nExactMatch = EXACT_MATCH_SECOND;
                        }
                    }
                    continue ;
                }
                /*3 如果第二个命令字段是变量类型，而且片断不是与两个命令都匹配*/                
                if ( CMD_VARIABLE( cmd_second ) )
                {
                    if ( *pWhichElement != WHICH_MATCH_BOTH )
                    {
                        match1 = cmd_piece_match( cmd_first , strcmd );
                        if ( match1 >= extend_match && match1 < exact_match )
                        {
                            *pWhichElement = WHICH_MATCH_BOTH;
                            *pExactConflict = CONFLICT_MATCH_FIRST; /*目前是关键字优先匹配，因此最佳匹配第一个字段*/
                        }
                        else if ( match1 == exact_match )
                        {
                            *pWhichElement = WHICH_MATCH_BOTH;
                            *pExactConflict = CONFLICT_MATCH_FIRST;
                            if ( nExactMatch == EXACT_MATCH_SECOND )
                            {
                                *pExactConflict = CONFLICT_NO;
                                return 0;
                            }
                            nExactMatch = EXACT_MATCH_FIRST;
                        }
                    }
                    continue ;
                }
            }
            /*4 这里是关键字匹配问题了*/
            len_cmd_first = VOS_StrLen( cmd_first );
            len_cmd_second = VOS_StrLen( cmd_second );
            len_cmd_min = ( len_cmd_first < len_cmd_second ? len_cmd_first : len_cmd_second );
            /*4.1 一个关键字包含另一个关键字，例如abc和abc或abcd*/
            if ( VOS_StrnCmp( cmd_first , cmd_second , len_cmd_min ) == 0 )
            {
                if ( VOS_StrnCmp( cmd_first, strcmd, strLen ) == 0 &&
                        VOS_StrnCmp( cmd_second, strcmd, strLen ) == 0 )   /*与两个匹配，比较下一个命令片断*/
                {
                    *pWhichElement = WHICH_MATCH_BOTH;
                    if ( strLen == len_cmd_min )   /*输入是abc*/
                    {
                        if ( strLen == len_cmd_first && strLen != len_cmd_second )
                        {
                            *pExactConflict = CONFLICT_MATCH_FIRST; /*与第一个最佳匹配*/
                            nExactMatch = EXACT_MATCH_FIRST; /*与第一个最佳匹配*/
                        }
                        else if ( strLen == len_cmd_second && strLen != len_cmd_first )
                        {
                            *pExactConflict = CONFLICT_MATCH_SECOND;
                            nExactMatch = EXACT_MATCH_SECOND;
                        }
                        else
                        {
                            nExactMatch = EXACT_MATCH_NO; /*两个关键字都是一样的，比如abc和abc，输入也是abc*/
                        }
                    }
                    else /*输入是ab*/
                    {
                        if ( len_cmd_first != len_cmd_second )   /*关键字是abc和abcd*/
                        {
                            *pExactConflict = CONFLICT_YES;
                        }
                        else
                        {
                            ; /*两个关键字都是一样的，比如abc和abc，输入也是abc*/
                        }
                    }
                }
            }
            else
            {
                /*4.2 两个命令字段不一样，例如abc和abd*/
                if ( VOS_StrCmp( cmd_first, cmd_second ) != 0 )
                {
                    /*4.2.1 输入片断和两个命令字段都匹配，比如输入ab*/
                    if ( VOS_StrnCmp( cmd_first, strcmd, strLen ) == 0 &&
                            VOS_StrnCmp( cmd_second, strcmd, strLen ) == 0 )   /*模糊命令*/
                    {
                        *pWhichElement = WHICH_MATCH_BOTH; /*两个命令都匹配*/
                        *pExactConflict = CONFLICT_YES; /*有冲突*/
                        return 1;
                    }
                    if ( VOS_StrnCmp( cmd_first, strcmd, strLen ) == 0 )
                    {
                        if ( *pWhichElement == WHICH_MATCH_SECOND )
                        {
                            *pExactConflict = CONFLICT_YES;
                            return 1;
                        }
                        else
                        {
                            *pWhichElement = WHICH_MATCH_FIRST;
                        }
                    }
                }
            }
        }
    }

    if ( *pWhichElement == WHICH_MATCH_NO )   /*不和其中的任何一个匹配*/
    {
        return -1;
    }

    if ( nExactMatch == EXACT_MATCH_FIRST || nExactMatch == EXACT_MATCH_SECOND )
    {
        *pWhichElement = WHICH_MATCH_BOTH;
        if ( ( no_match != match1_max ) &&
                ( match1_max == match2_max )
           )
        {
            *pExactConflict = CONFLICT_NO;
        }
        else
        {
            *pExactConflict = nExactMatch ;
        }
    }

    if ( *pExactConflict == CONFLICT_YES )
    {
        return 1;
    }

    return 0 ;
}


/**************************************************
通过用户输入命令来判断两个注册命令是否冲突
注意: 必须保证两个命令元素的长度比nStringFieldNum的长。
vline已经和两个命令元素匹配。
pExactConflict = 0,无冲突， ＝ 1冲突；2 与第一个最佳匹配，3与第二个最佳匹配; 4 nStringFieldNum －1字段模糊匹配
return value: -1 error , 0 show not conflicted, 1 shwo have conflicted
代码优化:每次只比较vline的最后一个片断，根据此片断的比较结果来判断谁是最优命令
----------------------------------------------------*/
LONG compare_partly_string_field_num(struct vty *vty, cl_vector vline, struct cmd_element * cmd_element_first, struct cmd_element * cmd_element_second, ULONG nStringFieldNum , LONG * pWhichElement , LONG * pExactConflict )
{
    cl_vector strvec_first = NULL;
    cl_vector strvec_second = NULL;
    cl_vector cmd_vector_first = NULL;
    cl_vector cmd_vector_second = NULL;
    cl_vector match_vector_first = NULL ;
    cl_vector match_vector_second = NULL ;
    CHAR *strcmd = NULL ;
    ULONG i = 0;
    LONG iRet = 0 ;

    CHAR *str = NULL;

    *pWhichElement = WHICH_MATCH_NO;
    *pExactConflict = CONFLICT_NO;

    /*************************************/
#define Match_Free_Vector    \
{    if ( match_vector_first ) cl_vector_free(match_vector_first) ; \
if (match_vector_second ) cl_vector_free(match_vector_second) ; }
    /*************************************/

    match_vector_first = cmd_get_match_piece_vector( vty, vline , cmd_element_first );
    match_vector_second = cmd_get_match_piece_vector(vty, vline , cmd_element_second );
    if ( match_vector_first == NULL || match_vector_second == NULL )
    {
        Match_Free_Vector
        return -1;
    }

    strvec_first = match_vector_first;
    strvec_second = match_vector_second;
    if ( cl_vector_max( strvec_first ) < nStringFieldNum ||
            cl_vector_max( strvec_second ) < nStringFieldNum )
    {
        Match_Free_Vector
        return -1;
    }

    i = nStringFieldNum-1;    
    cmd_vector_first = cl_vector_slot( strvec_first, i );
    cmd_vector_second = cl_vector_slot( strvec_second, i );
    if ( ( cmd_vector_first == NULL ) || ( cmd_vector_second == NULL ) )
    {
        Match_Free_Vector
        return -1;
    }
    str = cl_vector_slot( vline, i );
    if ( str == NULL )
    {
        Match_Free_Vector
        return -1;
    }

    strcmd = VOS_StrDupToLower( str, MODULE_RPU_CLI );/*小写化*/
    if ( strcmd == NULL )
    {
        Match_Free_Vector
        return -1;
    }
    iRet = compare_partly_string( strcmd, cmd_vector_first, cmd_vector_second, pWhichElement, pExactConflict );
    VOS_Free( strcmd );
    strcmd = NULL ;   

    Match_Free_Vector
    return iRet ;
}

/*************************************************
节点索引下存储的是字符串
-------------------------------------------------*/
VOID cmd_FreeStrVector(cl_vector pstStrVec)
{
    LONG index = 0;
    CHAR *pTmpStr = NULL;
    VOS_ASSERT(NULL != pstStrVec);
    for(index = 0; index < (LONG)cl_vector_max(pstStrVec); index++)
    {
        pTmpStr = cl_vector_slot(pstStrVec, index);
        if(NULL != pTmpStr)
        {
            VOS_Free(pTmpStr);
        }
    }
    cl_vector_free(pstStrVec);
    return;    
}
/*------------------------------------------------------
上面是和解析规则相关的基本函数
********************************************************/



/*******************************************************
下面是解析规则的中间处理函数
-------------------------------------------------------*/

/************************************************
比较一段连续的命令字段和命令片断片断
这段连续的命令字段都是在大括号外或都是在大括号内
------------------------------------------------*/
enum match_type cmd_compare_paragraph (struct vty *vty, 
        cl_vector vline, cl_vector cmd_vec, 
        LONG vstart_index, LONG cmd_startindex, LONG cmp_length, LONG vline_complete,
        LONG * argc, CHAR **argv, LONG * nTrueVarag )
{
    enum match_type match = no_match;
    enum match_type ret = no_match;
    LONG i = 0;
    ULONG j = 0;
    CHAR *str = NULL;
    CHAR *low_str = NULL;
    CHAR *cmd = NULL;
    cl_vector descvec = NULL;
    struct desc *desc = NULL;
    ULONG cmd_index = 0;
    ULONG v_index = 0;
    LONG oldargc = *argc;
    LONG matched = 0;
    LONG matchword = MATCH_WORD_NO; /* 0 for novararg; 1 for variable  ;2 for keyword partly ;3 for keyword exact match  */
    CHAR *matchstr = NULL;
    ULONG ulFirstMatch = 1;    

    *nTrueVarag = 0;

    for ( i = 0; i < cmp_length; i++ ) /*取出一个一个的字段*/
    {
        /*合法性检查*/
        cmd_index = cmd_startindex + i;
        if ( cmd_index >= cl_vector_max ( cmd_vec ) )
        {
            return no_match;
        }

        v_index = vstart_index + i;
        if ( v_index >= cl_vector_max ( vline ) )
        {
            if ( ( match ) || ( ( !match ) && ( i == 0 ) ) )
            {
                return incomplete_match;
            }
            else
            {
                return no_match;
            }
        }

        /*比较操作*/
        str = cl_vector_slot ( vline, v_index );
        descvec = cl_vector_slot ( cmd_vec, cmd_index );
        if ( ( !str ) && ( match ) )
        {
            return incomplete_match;
        }
        else if ( ( !str ) || ( !descvec ) )
        {
            return no_match;
        }

        matched = 0;
        matchword = MATCH_WORD_NO;
        for ( j = 0; j < cl_vector_max ( descvec ); j++ ) /*每一个字段(比如[abc|edf])中，含有多个子字段*/
        {
            desc = cl_vector_slot ( descvec, j );
            cmd = desc->cmd;
            if ( !cmd )
            {
                VOS_ASSERT(0);/*这种情况似乎不会发生!!!*/
                return no_match;/*ret = no_match;*/
            }
            else
            {
                low_str = VOS_StrDupToLower( str, MODULE_RPU_CLI );
                if ( low_str == NULL )
                {
                    return no_match;
                }
                ret = cmd_piece_match ( cmd, low_str );
                VOS_Free( low_str );
           }

            if ( ret )
            {
                matched++;
                /*在这里，目前匹配的优先级是: exact_match > partly_match > variable_match*/
                if ( CMD_VARIABLE( cmd ) ) 
                {
                    if ( (matchword != MATCH_WORD_EXACT) && (matchword != MATCH_WORD_PARTLY)) /*上一次不是最佳匹配和部分匹配*/
               /* if ( matchword != MATCH_WORD_EXACT)     */               
                    {
                        matchstr = str;
                        matchword = MATCH_WORD_VARIABLE;
                    }
                }
                else
                {
                	/* complete cmd line  */
                    if ( (0 != vline_complete) && (partly_match == ret) )
                    {
                        low_str = VOS_StrDup( cmd, MODULE_RPU_CLI );
                        if ( low_str != NULL )
                        {
                            cl_vector_set_index(vline, v_index, low_str);
                            VOS_Free( str );
                        }
                    }
					
                    
                    /* exact_match or partly_match*/
                    if ( desc->multi == 1 ) /* 中括号内*/
                    {
                        if ( ret == exact_match ) /*当前是完全匹配*/
                        {
                            matchstr = cmd;
                            matchword = MATCH_WORD_EXACT;
                        }
                        else if ( matchword == MATCH_WORD_PARTLY) /*上一次是部分匹配，本次还是部分匹配，因此是模糊匹配*/
                        {
                            return ambiguous_match;
                        }
                        else if ( matchword != MATCH_WORD_EXACT) /*上一次不是最佳匹配*/
                        {
                            matchstr = cmd;
                            matchword = MATCH_WORD_PARTLY;
                        }
                    }
                }
            }

            if ( ( matched ) && ( ret >= extend_match ) )
            {
                if ( ret == vararg_match )
                {
                    *nTrueVarag = 1;
                }

                if ( ulFirstMatch )
                {
                    ulFirstMatch = 0;
                    match = ret;
                }
                if ( ( ret < match ) && ( match != vararg_match ) )
                {
                    match = ret;
                }
                if ( ( ret == vararg_match ) && ( match >= extend_match ) )
                {
                    match = vararg_match;
                }
            }  
        }

        if ( matchword )
        {
            if ( _CL_CMD_ARGC_MAX_ -1 < *argc )   /*防止溢出*/
            {
                return no_match;
            }
            argv[ *argc ] = matchstr;
            *argc = *argc + 1;
        }
        if ( !matched )
        {
            *argc = oldargc;
            *nTrueVarag = 0;
            return no_match;
        }
    }

    return match;
}


/****************************************************************
 比较整条命令
 比较 vline 和 命令元素cmd_element是否匹配。argc，argv返回相应的命令行参数
 处理方式类似cmd_entry_function_describe 
----------------------------------------------------------------*/
enum match_type cmd_compare_one_element (struct vty *vty, cl_vector vline,
        LONG vline_complete,
        struct cmd_element * cmd_element,
        LONG * argc, CHAR **argv )
{
    ULONG i = 0;
    LONG vline_piece_index = 0;
    LONG cmd_piece_index = 0;
    LONG compare_length = 0;
    LONG sub_times = 0;
    LONG vline_left_length = 0;
    LONG real_compare_length = 0;
    LONG oldargc = *argc;
    cl_vector cmd_cl_vector = NULL;
    cl_vector sub_cl_vector = NULL;
    struct subcmd *sub_cmd = NULL;
    enum match_type match  = exact_match;/*记录所有匹配的最低级别*/
    enum match_type submatch_highest = no_match; /*避免 "test { key1 key2}*1"在输入"test key1"的时候无法返回正确的提示*/
    /*submatch_highest值为 no_match, incomplete_match, ambiguous_match之一 */
    enum match_type ret = no_match;
    LONG one_argc = 0; /*中间变量，避免修改argc，argv*/
    CHAR *one_argv[ _CL_CMD_ARGC_MAX_ ] = {0};

    LONG nTrueVarag = 0;

    sub_cl_vector = cmd_element->subconfig;
    cmd_cl_vector = cmd_element->strvec;
    cmd_piece_index = 0;
    vline_piece_index = 0;

    match = no_match;

    one_argc = 0;
    VOS_MemZero(one_argv, sizeof(one_argv));
    if ( sub_cl_vector )
    {
        /*比较所有可选的子串*/
        for ( i = 0; i < cl_vector_max ( sub_cl_vector ); i++ )
        {
            sub_cmd = cl_vector_slot ( sub_cl_vector, i );

            /*compare command before subcmdstart;比较可选部分之前的内容 */
            compare_length = ( sub_cmd->head_index ) - cmd_piece_index;

            if ( compare_length > 0 )
            {
                ret = cmd_compare_paragraph ( vty, vline, cmd_cl_vector, vline_piece_index,
                                              cmd_piece_index, compare_length, vline_complete, argc, argv, &nTrueVarag );
                if ( ret < extend_match )
                {
                    if(ret < submatch_highest)
                    {
                        ret = submatch_highest;
                    }
                    return ret;
                }
                else
                {
                    if ( i == 0 )
                    {
                        match = ret;
                    }
                    else if ( ret < match )
                    {
                        match = ret;    /*match 存储最小匹配程度*/
                    }

                    cmd_piece_index = cmd_piece_index + compare_length;
                    vline_piece_index = vline_piece_index + compare_length;
                }
            }

            if ( !( sub_cmd->permute ) )
            {
                /*compare command subcmd;比较可选部分 */
                sub_times = 0;
                compare_length = sub_cmd->tail_index - sub_cmd->head_index;


                if ( cl_vector_max( vline ) < ( ULONG ) ( vline_piece_index + compare_length ) )
                {
                    /* 肯定不会匹配的，最多返回incomplete_match，不传入argc,argv,改传入one_argc ,one_argv.
                    * 之所以继续比较，仅仅是为了得到匹配的类型，避免 "test { key1 key2}*1"在输入"test key1"的时候无法返回正确的提示
                    */
                    ret = cmd_compare_paragraph ( vty, vline, cmd_cl_vector, vline_piece_index,
                                                  cmd_piece_index, compare_length, vline_complete, &one_argc, one_argv, &nTrueVarag );
                    if ( submatch_highest < ret )  /*避免 "test { key1 key2}*1"在输入"test key1"的时候无法返回正确的提示*/
                    {
                        submatch_highest = ret;
                    }
                }
                else
                {
                    ret = cmd_compare_paragraph (vty, vline, cmd_cl_vector, vline_piece_index,
                                                  cmd_piece_index, compare_length, vline_complete, argc, argv, &nTrueVarag );
                }
                if ( ret < extend_match )  /*跳出本大括号，和下一个 com1 {com2}进行比较*/
                {
                    cmd_piece_index = cmd_piece_index + compare_length;
                    continue;
                }
                else
                {
                    if ( i == 0 )
                    {
                        match = ret;
                    }
                    else if ( ret < match )
                    {
                        match = ret;
                    }

                    sub_times++;
                    vline_piece_index = vline_piece_index + compare_length;
                }
                while ( ( ret > incomplete_match ) && ( sub_times < sub_cmd->max_times ) )
                {

                    if ( cl_vector_max( vline ) <= ( ULONG ) ( vline_piece_index ) )
                    {
                        break; /*匹配了*/
                    }

                    if ( cl_vector_max( vline ) < ( ULONG ) ( vline_piece_index + compare_length ) )
                    {
                        /* 肯定不会匹配的，最多返回incomplete_match，不传入argc,argv,改传入one_argc ,one_argv.
                        * 之所以继续比较，仅仅是为了得到匹配的类型，避免 "test { key1 key2}*1"在输入"test key1"的时候无法返回正确的提示
                        */
                        ret = cmd_compare_paragraph (vty, vline, cmd_cl_vector, vline_piece_index,
                                                      cmd_piece_index, compare_length, vline_complete, &one_argc, one_argv, &nTrueVarag );
                        if ( submatch_highest < ret )  /*避免 "test { key1 key2}*1"在输入"test key1"的时候无法返回正确的提示*/
                        {
                            submatch_highest = ret;
                        }
                    }
                    else
                    {
                        ret = cmd_compare_paragraph ( vty, vline, cmd_cl_vector, vline_piece_index,
                                                      cmd_piece_index, compare_length, vline_complete, argc, argv, &nTrueVarag );
                    }

                    if ( ret < extend_match )
                    {
                        /*cmd_piece_index = cmd_piece_index + compare_length;*/
                        break; /*比较下一个{}   */ /*continue;*/
                    }
                    else
                    {
                        if ( i == 0 )
                        {
                            match = ret;
                        }
                        else if ( ret < match )
                        {
                            match = ret;
                        }

                        sub_times++;
                        vline_piece_index = vline_piece_index + compare_length;
                    }
                }

                cmd_piece_index = cmd_piece_index + compare_length;
            }

            else /* sub_cmd->permute*/
            {
                sub_times = 0;
                compare_length = sub_cmd->tail_index - sub_cmd->head_index;

                ret = cmd_permute_compare_paragraph ( vline, cmd_cl_vector, vline_piece_index,
                                                      cmd_piece_index, compare_length, argc, argv, &real_compare_length );

                if ( ret < extend_match )
                {
                    cmd_piece_index = cmd_piece_index + compare_length;
                    continue;
                }
                else
                {
                    if ( i == 0 )
                    {
                        match = ret;
                    }
                    else if ( ret < match )
                    {
                        match = ret;
                    }

                    sub_times++;
                    vline_piece_index = vline_piece_index + compare_length;
                }

                cmd_piece_index = cmd_piece_index + compare_length;
            } /*end : if !sub_cmd->permute*/

        }

        /* compare command after all subcmd
         如: ping {-t} <A.B.C.D>,比较完 {-t} 后，接着比较<A.B.C.D> */

        if ( ( cl_vector_max( cmd_cl_vector ) == ( ( ULONG ) sub_cmd->tail_index ) ) && ( cl_vector_max( cmd_cl_vector ) == ( ( ULONG ) vline_piece_index ) ) )
        { /*垃圾*/
            if ( cl_vector_max( vline ) == cl_vector_max( cmd_cl_vector ) )
            {
                return match;
            }
        }

        vline_left_length = vline->max - vline_piece_index;
        compare_length = cl_vector_max ( cmd_cl_vector ) - cmd_piece_index;

        if ( compare_length > 0 )
        {
            ret = cmd_compare_paragraph (vty, vline, cmd_cl_vector, vline_piece_index,
                                          cmd_piece_index, compare_length, vline_complete, argc, argv, &nTrueVarag );

            if ( ret == ambiguous_match )
            {
                *argc = oldargc;
                return ambiguous_match;
            }
            if ( ( ret == no_match ) || ( ret == incomplete_match ) )
            {
                *argc = oldargc;
                if ( no_match < submatch_highest )  /*避免 "test { key1 key2}*1 key3"在输入"test key1"的时候无法返回正确的提示*/
                {
                    return submatch_highest;
                }
                return ret;
            }
            if ( ret >= extend_match )
            {
                if ( i == 0 )
                {
                    match = ret;
                }
                else if ( ret < match )
                {
                    match = ret;
                }
            }
        }

        if ( nTrueVarag )
        {
            LONG j;
            for ( j = 0; j < ( vline_left_length - compare_length );j++ )
            {
                CHAR *str;
                str = cl_vector_slot ( vline, ( vline_piece_index + compare_length + j ) );
                argv[ *argc ] = str;
                *argc = *argc + 1;
                if ( *argc >= ( _CL_CMD_ARGC_MAX_ ) )
                {
                    *argc = oldargc; 
                    return no_match;
                }
            }
        }
        else
        {
            if ( vline_left_length > compare_length )
            {
                *argc = oldargc;

                if ( ret == ambiguous_match )  /*可选字段，解决show run st 的问题*/
                {
                    return ambiguous_match;
                }
                if ( no_match < submatch_highest )  /*避免 "test { key1 key2}*1"在输入"test key1"的时候无法返回正确的提示*/
                {
                    return submatch_highest;
                }
                if ( ret == incomplete_match )  /*可选字段，仅仅匹配一部分:如 vl 匹配 {vlan <name>}*/
                {
                    return incomplete_match;
                }
                return no_match;
            }
        }

        if ( ( match == no_match ) || ( match == incomplete_match ) || ( match == ambiguous_match ) )
        {
            *argc = oldargc;
        }

        if ( match < submatch_highest )  /*避免 "test { key1 key2}*1"在输入"test key1"的时候无法返回正确的提示*/
        {
            return submatch_highest;
        }

        return match;
    }
    /* if don't have subcmd  */
    else
    {
        vline_left_length = vline->max - vline_piece_index;
        compare_length = cl_vector_max ( cmd_cl_vector ) - cmd_piece_index;

        match = cmd_compare_paragraph (vty, vline, cmd_cl_vector, vline_piece_index,
                                        cmd_piece_index, compare_length, vline_complete, argc, argv, &nTrueVarag );

        if ( nTrueVarag )
        {
            LONG k;
            for ( k = 0; k < ( vline_left_length - compare_length );k++ )
            {
                CHAR *str;
                str = cl_vector_slot ( vline, ( vline_piece_index + compare_length + k ) );
                argv[ *argc ] = str;
                *argc = *argc + 1;
                if ( *argc >= ( _CL_CMD_ARGC_MAX_ ) )
                {
                    *argc = oldargc;
                    return no_match;
                }
            }
        }
        else
        {
            if ( vline_left_length > compare_length )
            {
                *argc = oldargc;
                if ( ret == incomplete_match )
                {
                    return incomplete_match;
                }
                return no_match;
            }
        }
        if ( ( match == no_match ) || ( match == incomplete_match ) || ( match == ambiguous_match ) )
        {
            *argc = oldargc;
        }

        return match;
    }
}

/************************************************
'?'查询帮助处理函数
命令的处理分为:
含有大括号,比如 aaa {bbb} ccc {ddd} eee         和
没有大括号,比如 aaa bbb ccc       
 
含有大括号的处理方式:
----------------------------------------
    coms1  {coms2}      coms3
----------------------------------------
    |    循环处理     |      最后处理  
 
不含大括号的一次性处理
 
返回: 1 ok； 0 fail。
 
参数 matchvec存储所有匹配的命令字段
       lExecuteStatus 表明此命令是否可以执行 0 不行；1可以
 
 考虑去掉vty参数!!!
-------------------------------------------------*/
LONG cmd_entry_function_describe ( struct vty * vty, struct cmd_element * cmd_element, cl_vector vline, LONG * lExecuteStatus, cl_vector matchvec )
{
    ULONG i, j, k, l;
    LONG vline_piece_index;
    LONG cmd_piece_index;
    LONG compare_length;
    LONG last_item_index = 0;
    LONG real_compare_length = 0;
    LONG sub_times;
    LONG vline_left_length;
    LONG output_left = 1;
    cl_vector sub_cl_vector;
    struct subcmd *sub_cmd;
    enum match_type match;
    enum match_type ret;

    CHAR *temp_match_str = NULL;

    CHAR *cmd;
    CHAR *user_str;
    CHAR *match_str;
    cl_vector match_item_vec;

    cl_vector cmd_cl_vector;
    cl_vector descvec;
    struct desc *desc;
    LONG matched;
    LONG item_match;
    LONG next_sub;
    LONG argc = 0;
    CHAR *argv[_CL_CMD_ARGC_MAX_] = {0};
    LONG nTrueVarag = 0;
    UCHAR flag = 0;
    static struct desc desc_cr =
        { "<cr>", "Just press enter to execute command!" , 0 , 0
        };

    sub_cl_vector = cmd_element->subconfig;
    cmd_cl_vector = cmd_element->strvec;
    cmd_piece_index = 0;
    vline_piece_index = 0;

    match = exact_match;
    *lExecuteStatus = 0;
    if ( sub_cl_vector )
        /*
        If there is sub command  { } * n
        */
    {
        for ( i = 0; i < cl_vector_max ( sub_cl_vector ); i++ )
        {
            sub_cmd = cl_vector_slot ( sub_cl_vector, i );
            /*
            compare command before subcmdstart;
            */
            compare_length = ( sub_cmd->head_index ) - cmd_piece_index;
            vline_left_length = vline->max - vline_piece_index - 1;

            if ( vline_left_length == 0 )
            {
                if ( compare_length > 0 )
                {
                    goto check_partly_match;
                }
                else
                {}
            }

            if ( compare_length )
            {
                ret =
                    cmd_compare_paragraph ( vty, vline, cmd_cl_vector, vline_piece_index,
                                                   cmd_piece_index, ( ( compare_length > vline_left_length ) ? vline_left_length : compare_length ), 0, &argc, argv, &nTrueVarag );
                if ( ret <= incomplete_match )
                {
                    goto check_no_match;
                }
                else
                {
                    cmd_piece_index = cmd_piece_index + ( ( compare_length > vline_left_length ) ? vline_left_length : compare_length );
                    vline_piece_index = vline_piece_index + ( ( compare_length > vline_left_length ) ? vline_left_length : compare_length );
                    if ( compare_length > vline_left_length )  /*比较结束*/
                    {
                        goto check_partly_match;
                    }
                }
            }
            /****End compare command before subcmdstart****/

            /*
            compare command subcmd;
            */
            /* 如果是可重复的子命令 */
            if ( !( sub_cmd->permute ) )
            {
                sub_times = 0;
                next_sub = 1;
                while ( ( next_sub ) && ( sub_times < sub_cmd->max_times ) )
                {
                    compare_length = sub_cmd->tail_index - sub_cmd->head_index;
                    vline_left_length = vline->max - vline_piece_index - 1; /*最后一个命令片断不参与比较*/
                    if ( vline_left_length == 0 )  /*比较结束，记录下最后一个命令字段和相应的描述说明*/
                    {
                        descvec = cl_vector_slot( cmd_cl_vector, cmd_piece_index );
                        user_str = cl_vector_slot( vline, vline_piece_index );

                        matched = 0;
                        for ( j = 0; j < cl_vector_max ( descvec ); j++ )
                        {
                            desc = cl_vector_slot ( descvec, j );
                            cmd = desc->cmd;
                            if ( user_str == NULL )    /*if(user_str == '\0')*/
                            {
                                match_str = cmd;
                                if ( !desc_unique_string ( vty, matchvec, match_str ) )
                                {
                                    cl_vector_set ( matchvec, desc );
                                }
                                matched++;
                            }
                            else
                            {
                                if ( !cmd )
                                {
                                    VOS_ASSERT( 0 ); /*看看是否会发生*/
                                    ret = no_match;
                                }
                                else
                                {
                                    ret = cmd_piece_match ( cmd, user_str );

                                    if ( ret )
                                    {
                                        match_str = cmd;
                                        if ( !desc_unique_string ( vty, matchvec, match_str ) )
                                        {
                                            cl_vector_set ( matchvec, desc );
                                        }
                                        matched++;
                                    }
                                }
                            }
                        };
                        next_sub = 0;
                    }
                    else /*比较大括号*/
                    {
                        ret =
                            cmd_compare_paragraph ( vty, vline, cmd_cl_vector, vline_piece_index,
                                                           cmd_piece_index, ( ( compare_length > vline_left_length ) ? vline_left_length : compare_length ), 0, &argc, argv, &nTrueVarag );

                        if ( !ret )  /*大括号比较结束，检查一下比较次数*/
                        {
                            if ( sub_times < sub_cmd->min_times )
                            {
                                goto check_no_match;
                            }
                            next_sub = 0;
                        }
                        else
                        {                          
                            if ( compare_length > vline_left_length )  /*比较结束*/
                            {
                                descvec = cl_vector_slot( cmd_cl_vector, cmd_piece_index + vline_left_length );
                                user_str = cl_vector_slot( vline, vline_piece_index + vline_left_length );

                                matched = 0;
                                for ( j = 0; j < cl_vector_max ( descvec ); j++ )
                                {
                                    desc = cl_vector_slot ( descvec, j );
                                    cmd = desc->cmd;
                                    if ( user_str == NULL )    /*if(user_str == '\0')*/
                                    {
                                        match_str = cmd;
                                        if ( !desc_unique_string ( vty, matchvec, match_str ) )
                                        {
                                            cl_vector_set ( matchvec, desc );
                                        }
                                        matched++;
                                    }
                                    else
                                    {
                                        if ( !cmd )
                                        {
                                            ret = no_match;
                                        }
                                        else
                                        {
                                            ret = cmd_piece_match ( cmd, user_str );

                                            if ( ret )
                                            {
                                                match_str = cmd;
                                                if ( !desc_unique_string ( vty, matchvec, match_str ) )
                                                {
                                                    cl_vector_set ( matchvec, desc );
                                                }
                                                matched++;
                                            }
                                        }
                                    }
                                };
                                next_sub = 0;
                            }
                            else
                            {
                                vline_piece_index = vline_piece_index + compare_length;
                                sub_times++;
                            }
                        }
                    }
                }
                cmd_piece_index = cmd_piece_index + sub_cmd->tail_index - sub_cmd->head_index;
            }
            else /*圆括号处理, 不会运行到*/
            {
                /*sub_times = 0;
                next_sub = 1;*/
                VOS_ASSERT( 0 );
                matched = 0;
                compare_length = sub_cmd->tail_index - sub_cmd->head_index;
                vline_left_length = vline->max - vline_piece_index - 1;
                user_str = cl_vector_slot( vline, cl_vector_max( vline ) - 1 );
                match_item_vec = cl_vector_init( INIT_MATCHVEC_SIZE );
                if ( NULL == match_item_vec )
                {
                    VOS_ASSERT( 0 );
                    VOS_Free( temp_match_str );
                    return 0;
                }

                ret =
                    cmd_permute_partly_compare_paragraph ( vline, cmd_cl_vector, vline_piece_index,
                                                           cmd_piece_index, compare_length, &temp_match_str, &real_compare_length, match_item_vec, &last_item_index );

                /* 当返回值为匹配的时候，可能全匹配命令的后面还有命令 */
                /* 如果全匹配后面还有命令，给ret赋值no_match,直接跳过 */
                /* 全匹配命令的处理分支。                             */
                if ( ret )
                {
                    /* 将子命令中没有输入的各项都打印出来。*/
                    matched = 0;
                    l = 0;
                    for ( k = 0; ( LONG ) k < compare_length; k++ )
                    {
                        descvec = cl_vector_slot( cmd_cl_vector, cmd_piece_index + k );
                        for ( j = 0; j < cl_vector_max( descvec ); j++ )
                        {
                            desc = cl_vector_slot( descvec, j );
                            if ( l < cl_vector_max( match_item_vec ) && cl_vector_slot( match_item_vec, l ) )
                            {
                                matched++;
                            }
                            if ( desc->multi == 3 )
                            {
                                l += cl_vector_max( descvec );
                                break;
                            }
                            l++;
                        }
                    }
                    for ( j = vline_piece_index; j < cl_vector_max( vline ); j++ )
                    {
                        if ( cl_vector_slot( vline, j ) )
                        {
                            matched--;
                        }
                    }
                    if ( matched != 0 )
                    {
                        ret = no_match;
                    }
                }
                matched = 0;
                if ( ret || ( !ret && vline_left_length == 0 && !user_str ) )
                {
                    output_left = 0;
                    if ( temp_match_str )
                    {
                        VOS_Free( temp_match_str );
                    }

                    /* 以下一段代码判断最后一个匹配的命令分片是在 */
                    /* 全匹配命令项的第一到最后一个命令分片之间   */
                    /* (不包括最后一个命令分片),还是最后一个命令分片。 */
                    item_match = 0;
                    if ( cl_vector_max( match_item_vec ) )
                    {
                        l = 0;
                        j = 0;
                        for ( k = 0; k < ( ULONG ) compare_length; k++ )
                        {
                            descvec = cl_vector_slot( cmd_cl_vector, cmd_piece_index + k );
                            desc = cl_vector_slot ( descvec, 0 );
                            l = l + cl_vector_max( descvec );
                            if ( ( last_item_index == ( LONG ) l - 1 ) || ( ( desc->multi == 3 ) && ( ( ( LONG ) j <= last_item_index ) && ( last_item_index < ( LONG ) l ) ) ) )
                            {
                                item_match = 1;
                                break;
                            }
                            j = l;
                        }
                    }

                    /* 如果有一项没有匹配全，或者问号紧贴着输入，打印第一项没有匹配的命令分片。 */
                    if ( ( !item_match && cl_vector_max( match_item_vec ) ) || user_str )
                    {
                        if ( !user_str )
                        {
                            last_item_index++;
                        }
                        l = 0;
                        j = 0;
                        for ( k = 0; k < ( ULONG ) compare_length; k++ )
                        {
                            descvec = cl_vector_slot( cmd_cl_vector, cmd_piece_index + k );
                            l += cl_vector_max( descvec );
                            if ( ( LONG ) j <= last_item_index && last_item_index < ( LONG ) l )
                            {
                                desc = cl_vector_slot ( descvec, 0 );
                                /* 如果是多选一项，并且上一匹配的命令分片正好也在这一项内。 */
                                if ( ( desc->multi == 3 ) && ( !user_str && ( ( LONG ) j <= last_item_index - 1 ) && ( last_item_index - 1 < ( LONG ) l ) ) )
                                {
                                    k = compare_length;
                                }
                                break;
                            }
                            j = l;
                        }
                        if ( k < ( ULONG ) compare_length )
                        {
                            descvec = cl_vector_slot( cmd_cl_vector, cmd_piece_index + k );

                            desc = cl_vector_slot ( descvec, cl_vector_max( descvec ) - ( l - last_item_index ) );
                            cmd = desc->cmd;
                            if ( user_str == NULL )    /*if(user_str == '\0')*/
                            {
                                match_str = cmd;
                                if ( !desc_unique_string ( vty, matchvec, match_str ) )
                                {
                                    cl_vector_set ( matchvec, desc );
                                }
                                matched++;
                            }
                            else
                            {
                                if ( !cmd )
                                {
                                    ret = no_match;
                                }
                                else
                                {
                                    ret = cmd_piece_match ( cmd, user_str );

                                    if ( ret )
                                    {
                                        match_str = cmd;
                                        if ( !desc_unique_string ( vty, matchvec, match_str ) )
                                        {
                                            cl_vector_set ( matchvec, desc );
                                        }
                                        matched++;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        if ( item_match )
                        {
                            output_left = 1;
                        }
                        l = 0;
                        for ( k = 0; k < ( ULONG ) compare_length; k++ )
                        {
                            descvec = cl_vector_slot( cmd_cl_vector, cmd_piece_index + k );

                            matched = 0;
                            for ( j = 0; j < cl_vector_max ( descvec ); j++ )
                            {
                                if ( ( l >= cl_vector_max( match_item_vec ) ) || ( ( l < cl_vector_max( match_item_vec ) ) && !cl_vector_slot( match_item_vec, l ) ) )
                                {
                                    desc = cl_vector_slot ( descvec, j );
                                    cmd = desc->cmd;
                                    if ( user_str == NULL )   /*if(user_str == '\0')*/
                                    {
                                        match_str = cmd;
                                        if ( !desc_unique_string ( vty, matchvec, match_str ) )
                                        {
                                            cl_vector_set ( matchvec, desc );
                                        }
                                        matched++;
                                    }
                                    else
                                    {
                                        if ( !cmd )
                                        {
                                            ret = no_match;
                                        }
                                        else
                                        {
                                            ret = cmd_piece_match ( cmd, user_str );

                                            if ( ret )
                                            {
                                                match_str = cmd;
                                                if ( !desc_unique_string ( vty, matchvec, match_str ) )
                                                {
                                                    cl_vector_set ( matchvec, desc );
                                                }
                                                matched++;
                                            }
                                        }
                                    }
                                    if ( desc->multi != 3 )
                                    {
                                        break;
                                    }
                                }
                            };
                            l += cl_vector_max( descvec );
                        }
                    }
                }
                vline_piece_index = vline_piece_index + real_compare_length;

                cl_vector_free( match_item_vec );

                cmd_piece_index = cmd_piece_index + sub_cmd->tail_index - sub_cmd->head_index;
                if ( !matched )
                {
                    output_left = 1;
                }
            }

        }
        /*
        *  compare command after subcmd
        */
        vline_left_length = vline->max - vline_piece_index - 1;
        if ( vline_left_length < 0 )
        {
            vline_left_length = 0;
        }
        compare_length = cl_vector_max ( cmd_cl_vector ) - cmd_piece_index;
        /*
        if(vline_left_length > compare_length)
    {
            goto check_no_match;
    }
        */
        if ( vline_left_length == 0 )
        {
            goto check_partly_match;
        }

        ret = cmd_compare_paragraph (vty, vline, cmd_cl_vector, vline_piece_index, cmd_piece_index, ( ( compare_length > vline_left_length ) ? vline_left_length : compare_length ), 0, &argc, argv, &nTrueVarag );
        if ( ret )
        {
            cmd_piece_index = cmd_piece_index + ( ( compare_length > vline_left_length ) ? vline_left_length : compare_length );
            vline_piece_index = vline_piece_index + ( ( compare_length > vline_left_length ) ? vline_left_length : compare_length );
            if ( cmd_piece_index == ( LONG ) ( cl_vector_max( cmd_cl_vector ) ) )
            {
                if ( match == vararg_match )
                {
                    goto check_vararg_match;
                }

                user_str = cl_vector_slot( vline, vline_piece_index );
                if ( ( user_str == NULL ) || ( ret == vararg_match ) )       /*if((user_str == '\0')||(ret == vararg_match))*/
                {
                    goto check_full_match;
                }
                else
                {
                    goto check_no_match;
                }
            }
            else
            {
                goto check_partly_match;
            }
        }
        else
        {
            goto check_no_match;
        }
    }
    /*
    If there is no sub command { } * n
    */
    else
    {
        vline_left_length = vline->max - vline_piece_index - 1;
        compare_length = cl_vector_max ( cmd_cl_vector ) - cmd_piece_index;
        /*
        if(vline_left_length > compare_length)
    {
            goto check_no_match;
    }
        */
        if ( vline_left_length == 0 )
        {
            goto check_partly_match;
        }

        match =
            cmd_compare_paragraph ( vty, vline, cmd_cl_vector, vline_piece_index,
                                           cmd_piece_index, ( ( compare_length > vline_left_length ) ? vline_left_length : compare_length ), 0, &argc, argv, &nTrueVarag );

        if ( match )
        {           
            cmd_piece_index = cmd_piece_index + ( ( compare_length > vline_left_length ) ? vline_left_length : compare_length );
            vline_piece_index = vline_piece_index + ( ( compare_length > vline_left_length ) ? vline_left_length : compare_length );
            if ( cmd_piece_index == ( LONG ) ( cl_vector_max( cmd_cl_vector ) ) )
            {
                if ( match == vararg_match )
                {
                    goto check_vararg_match;
                }
                user_str = cl_vector_slot( vline, vline_piece_index );
                if ( user_str == NULL )     /*if(user_str == '\0')*/
                {
                    goto check_full_match;
                }
                else
                {
                    goto check_no_match;
                }
            }
            else
            {
                goto check_partly_match;
            }
        }
        else
        {
            goto check_no_match;
        }
    }

check_full_match:
    match_str = "<cr>";
    if ( !desc_unique_string ( vty, matchvec, match_str ) )
    {
        cl_vector_set ( matchvec, &desc_cr );
        *lExecuteStatus = 1; /*本命令可以执行*/
    }
    return 1;

check_vararg_match:  /*参数<.line>一般放在整个命令的最后面*/
    if ( NULL == cl_vector_slot ( vline, cl_vector_max( vline ) - 1 ) )
    {
        flag = 1;
        vline->max--;
    }
    argc = 0;
    VOS_MemZero(argv, sizeof(argv));
    ret = cmd_compare_one_element (vty, vline, 0, cmd_element, &argc, argv ); /*检查用户输入的参数个数*/
    if ( 1 == flag )
    {
        vline->max++;
    }

    if ( no_match == ret )  /*参数太多，返回0*/
    {
        return 0;
    }

    user_str = cl_vector_slot( vline, ( cl_vector_max( vline ) - 1 ) );
    /*
    if(user_str != '\0')
        goto check_partly_match;
    */
    match_str = "<cr>"; /*先插入回车键(提示可以执行命令)*/
    if ( !desc_unique_string ( vty, matchvec, match_str ) )
    {
        cl_vector_set ( matchvec, &desc_cr );
        *lExecuteStatus = 1; /*本命令可以执行*/
    }
    if ( _CL_CMD_ARGC_MAX_ -1 <= argc )   /*用户输入的参数达到极限，不再提示输入*/
    {
        return 1;
    }

    descvec = cl_vector_slot( cmd_cl_vector, ( cl_vector_max( cmd_cl_vector ) - 1 ) ); /*把命令的最后一个元素的所有选择取出*/

    matched = 0;
    for ( j = 0; j < cl_vector_max ( descvec ); j++ )
    {
        desc = cl_vector_slot ( descvec, j );
        cmd = desc->cmd;
        ret = cmd_piece_match ( cmd, user_str );

        if ( ret )
        {
            match_str = cmd;
            if ( !desc_unique_string ( vty, matchvec, match_str ) )
            {
                cl_vector_set ( matchvec, desc );
            }
            matched++;
        }
    };

    return 1;

check_partly_match:
    user_str = cl_vector_slot( vline, vline_piece_index );
    if ( cmd_piece_index >= ( LONG ) cl_vector_max( cmd_cl_vector ) )
    {
        if ( user_str == NULL )   /*if(user_str == '\0')*/
        {
            goto check_full_match;
        }
        else
        {
            goto check_no_match;
        }
    }
    if ( output_left )
    {
        descvec = cl_vector_slot( cmd_cl_vector, cmd_piece_index );

        matched = 0;
        for ( j = 0; j < cl_vector_max ( descvec ); j++ )
        {
            desc = cl_vector_slot ( descvec, j );
            cmd = desc->cmd;
            if ( user_str == NULL )         /*if(user_str == '\0')*/
            {
                match_str = cmd;
                if ( !desc_unique_string ( vty, matchvec, match_str ) )
                {
                    cl_vector_set ( matchvec, desc );
                }
                matched++;
            }
            else
            {
                if ( !cmd )
                {
                    ret = no_match;
                }
                else
                {
                    ret = cmd_piece_match ( cmd, user_str );

                    if ( ret )
                    {
                        match_str = cmd;
                        if ( !desc_unique_string ( vty, matchvec, match_str ) )
                        {
                            cl_vector_set ( matchvec, desc );
                        }
                        matched++;
                    }
                }
            }
        };
        if ( matched )
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 1;
    }
check_no_match:
    return 0;
}

/*************************************************
获得最后匹配用户输入命令(vline)的所有注册命令
输入参数:
cmd_cl_vector 当前节点(比如config节点)下的所有命令数组指针；
vline 用户输入命令
返回值:
0   正常(匹配0条到n条命令)
-1 冲突(模糊匹配)
-------------------------------------------------*/

/** 
 * 获得最后匹配用户输入命令(vline)的所有注册命令
 * @param[in]   vty             当前会话
 * @param[in]   cmd_cl_vector   当前节点(比如config节点)下的所有命令数组指针
 * @param[in]   vline           用户输入命令
 * @return      0 - 正常(匹配0条到n条命令)；-1 - 冲突(模糊匹配)
 */
LONG cmd_MatchCommands( struct vty *vty, cl_vector cmd_cl_vector, cl_vector vline )
{

    ULONG ulTmpMax = 0;
    ULONG ulMax = 0;
    LONG lLen = 0;
    LONG index = 0;
    LONG argc = 0;
    LONG lRet = 0; /*返回值，默认正常*/
    LONG WhichElement = WHICH_MATCH_BOTH;
    LONG ExactConflict = CONFLICT_NO;
    LONG lCmpResult = 0;
    enum match_type enMatchType = no_match;
    CHAR *argv[ _CL_CMD_ARGC_MAX_ ] = {0};    
    struct cmd_element *pstCmd = NULL;
    struct cmd_element *pstCmdFirst = NULL;
    LONG lCmdFirstIndex = 0;
    struct cmd_element *pstCmdSecond = NULL;
    LONG lCmdSecondIndex = 0;
    LONG lSameMatchLevelIndex = 0;/*匹配程度相同的所有命令的开始索引*/
    LONG lCurrentMatchCmdNum = 0; /*当前索引匹配的命令数*/

    if ( ( 0 == cl_vector_max( vline ) ) ||
         ( ( 1 == cl_vector_max( vline ) ) && ( NULL == cl_vector_slot( vline, 0 ) ) )
       )
    {
        return 0;
    }

    ulTmpMax = vline->max;
    if ( NULL == cl_vector_slot( vline, ulTmpMax - 1 ) )
    {
        ulMax = ulTmpMax - 1;
    }
    else
    {
        ulMax = ulTmpMax;
    }
    
    for ( lLen = 1; lLen <= ( LONG ) ulMax; lLen++ )   /*从输入命令的第一个片断匹配起,和所有的命令进行匹配*/
    {
        vline->max = lLen;
        lCurrentMatchCmdNum = 0;/*统计当前索引匹配的命令总数*/
        /* 暂时删除  tmp_del */
//        if((lLen == 1) && (VOS_StrLen(cl_vector_slot ( vline, 0 ))>30))
//        {
//            lRet = -2;
//            cmd_ShowAmbiguousOrUnknow(vty, vline, lLen -1, CMD_ERR_NO_MATCH);
//            goto exit;
//        }


        /*1.去掉不匹配的命令 */
        for ( index = 0; index < ( LONG ) cl_vector_max( cmd_cl_vector ); index++ )
        {
            pstCmd = cl_vector_slot( cmd_cl_vector, index );
            if ( NULL == pstCmd )
            {
                continue;
            }

            argc = 0;
            VOS_MemZero(argv, _CL_CMD_ARGC_MAX_*sizeof(CHAR*)); /*argv是个指针数组*/
            enMatchType = cmd_compare_one_element(vty, vline, 0, pstCmd, &argc, argv );
            if ( no_match == enMatchType )
            {
                cl_vector_unset( cmd_cl_vector, index );
            }
            else   /*这是一条已经匹配(且可能是模糊匹配的命令)*/
            {
                lCurrentMatchCmdNum++;/*统计当前索引匹配的命令总数*/
            }
        }
        
        if(0 == lCurrentMatchCmdNum) /*表明当前已经没有匹配的命令了,当前输入为unknow command*/
        {
            lRet = 0;
            cmd_ShowAmbiguousOrUnknow(vty, vline, lLen -1, CMD_ERR_NO_MATCH);
            goto exit;
        }
        
        /* 2.cmd_cl_vector中剩下的都是已经匹配的命令，下面比较匹配的程度 */
        pstCmdFirst = NULL;
        lCmdFirstIndex = 0;
        pstCmdSecond = NULL;
        lCmdSecondIndex = 0;
        for ( index = 0; index < ( LONG ) cl_vector_max( cmd_cl_vector ); index++ )
        {
            pstCmd = cl_vector_slot( cmd_cl_vector, index );
            if ( NULL == pstCmd )
            {
                continue;
            }

            if ( NULL == pstCmdFirst )
            {
                pstCmdFirst = pstCmd;
                lCmdFirstIndex = index;
                lSameMatchLevelIndex = index; /*记录下匹配级别相同的所有命令的第一个命令的索引*/
                continue;
            }
            pstCmdSecond = pstCmd;
            lCmdSecondIndex = index;

            lCmpResult = compare_partly_string_field_num( vty, vline, pstCmdFirst, pstCmdSecond, cl_vector_max( vline ), &WhichElement, &ExactConflict );
            if ( -1 == lCmpResult )  /*出现错误*/
            {
                VOS_ASSERT( 0 );
                lRet = -1;
                goto exit;
            }
            if ( ( 1 == lCmpResult ) || ( CONFLICT_YES == ExactConflict ) )  /*命令之间有冲突*/
            {
                lRet = -1;
                cmd_ShowAmbiguousOrUnknow(vty, vline, (LONG)cl_vector_max(vline)-1, CMD_ERR_AMBIGUOUS);
                goto exit;
            }
            /*去掉匹配程度低的命令，保留下最匹配的命令, pstCmdFirst始终指向最佳匹配命令*/
            if ( CONFLICT_MATCH_FIRST == ExactConflict )
            {
                cl_vector_unset( cmd_cl_vector, lCmdSecondIndex );
                lCmdSecondIndex = 0;
            }
            else if ( CONFLICT_MATCH_SECOND == ExactConflict )
            {
                while ( lSameMatchLevelIndex <= lCmdFirstIndex )  /*把前面和命令pstCmdFirst匹配级别相同的命令都去掉*/
                {
                    cl_vector_unset( cmd_cl_vector, lSameMatchLevelIndex );
                    lSameMatchLevelIndex++;
                }

                pstCmdFirst = pstCmdSecond;
                lCmdFirstIndex = lCmdSecondIndex;
                lSameMatchLevelIndex = lCmdSecondIndex;
                pstCmdSecond = NULL;
                lCmdSecondIndex = 0;
            }
            else
            {
                pstCmdFirst = pstCmdSecond; /*指针pstCmdFirst向下移动*/
                lCmdFirstIndex = lCmdSecondIndex;
                pstCmdSecond = NULL;
                lCmdSecondIndex = 0;
            }

        }
        /*3.最后查看一下每一条命令里是不是有中括号内模糊匹配的*/
        for ( index = 0; index < ( LONG ) cl_vector_max( cmd_cl_vector ); index++ )
        {
            pstCmd = cl_vector_slot( cmd_cl_vector, index );
            if ( NULL == pstCmd )
            {
                continue;
            }

            argc = 0;
            VOS_MemZero(argv, _CL_CMD_ARGC_MAX_*sizeof(CHAR*)); /*argv是个指针数组*/
            enMatchType = cmd_compare_one_element(vty, vline, 0, pstCmd, &argc, argv );
            if ( ambiguous_match == enMatchType )
            {
                lRet = -1;                
                /*指明模糊匹配位置*/
                cmd_ShowAmbiguousOrUnknow(vty, vline, (LONG)cl_vector_max(vline)-1, CMD_ERR_AMBIGUOUS);
                goto exit;
            }            
        }
       
    }

exit:
    vline->max = ulTmpMax;
    return lRet;
}


/************************************************
此函数负责执行一个已经匹配好的命令
------------------------------------------------*/
LONG cmd_ExecuteOneCommand( cl_vector vline, struct vty * vty, struct cmd_element * pstCmd )
{

    LONG ret = CMD_SUCCESS;
    LONG argc = 0;
    CHAR *argv[ _CL_CMD_ARGC_MAX_ ] = {0};
    struct cmd_element *pstCmdExecute = NULL;


    VOS_ASSERT( NULL != pstCmd );

    pstCmdExecute = pstCmd;	
	
    /*获取命令参数*/
    if(no_match == cmd_compare_one_element(vty, vline, 1, pstCmdExecute, &argc, argv ))
    {
        ret = CMD_WARNING;
        return ret;
    }

    if ( pstCmdExecute->msg_flag == DEFUN_FLAG /*||
              pstCmdExecute->p_que_id == 0 ||
              *( ULONG* ) pstCmdExecute->p_que_id == 0*/
       )
    {    	
        /* call it directly if this is config recovery time at system starting up */
        ret = ( *pstCmdExecute->func ) ( pstCmdExecute, vty, argc, argv );
    }
    else
    {
        switch ( pstCmdExecute->msg_flag )
        {
            case DEFUN_FLAG:
                ret = ( *pstCmdExecute->func ) ( pstCmdExecute, vty, argc, argv );
                break;
            case QDEFUN_FLAG:
                #if 1
                {
                    vty_out(vty,"QDEFUN is deprecated,please use DEFUN!!! \r\n");
                }
                #else
                {
                    LONG i;
                    struct cmd_arg_struct *pCmd;
                    SYS_MSG_S *pMsg = NULL;
                    ULONG ulMsg[ 4 ] = {0};
                    LONG ulRet = VOS_ERROR;
                    LONG iRecvMsgSerial = 0;

                    if ( ( pstCmdExecute->p_que_id == 0 ) ||
                            ( *( ULONG* ) pstCmdExecute->p_que_id == 0 ) )
                    {
                        vty_out( vty, "%% Command failed, maybe corresponding module is not started.\r\n" );
                        return CMD_ERR_NOTHING_TODO;
                    }

                    pCmd = ( struct cmd_arg_struct* ) VOS_Malloc( sizeof( struct cmd_arg_struct ), MODULE_RPU_CLI );
                    if ( pCmd == NULL )       /* add by liuyanjun 2002/08/26 */
                    {
                        VOS_ASSERT( 0 );
                        return CMD_ERR_NOTHING_TODO;
                    }                       /* end */

                    pCmd->matched_element = pstCmdExecute;
                    pCmd->vty = vty;
                    pCmd->current_time_ticks = VOS_GetTick();
                    pCmd->argc = argc;
                    {
                        LONG j = 0;
                        for ( i = 0; i < argc; i++ )
                        {
                            pCmd->argv[ i ] = VOS_StrDup ( argv[ i ] ,MODULE_RPU_CLI);
                            if ( pCmd->argv[ i ] == NULL )
                            {
                                for ( j = 0; j < i ; j++ )
                                    VOS_Free( pCmd->argv[ j ] );
                                VOS_Free( pCmd );
                                pCmd = NULL;
                                VOS_ASSERT( 0 );
                                return CMD_ERR_NOTHING_TODO;
                            }
                        }

                    }


                    pMsg = ( SYS_MSG_S* ) VOS_Malloc( sizeof( SYS_MSG_S ), MODULE_RPU_CLI );
                    if ( pMsg == NULL )       /* add by liuyanjun 2002/08/26 */
                    {
                        for ( i = 0; i < argc ; i++ )
                        {
                            if ( pCmd->argv[ i ] != NULL )
                            {
                                VOS_Free( pCmd->argv[ i ] );
                                pCmd->argv[ i ] = NULL ;
                            }
                        }
                        VOS_Free( pCmd );
                        pCmd = NULL;
                        /*ASSERT(0);*/
                        return CMD_ERR_NOTHING_TODO;
                    }                       /* end */

                    SYS_MSG_SRC_ID( pMsg ) = MODULE_RPU_CLI;
                    SYS_MSG_DST_ID( pMsg ) = 0;
                    SYS_MSG_MSG_TYPE( pMsg ) = MSG_REQUEST;
                    SYS_MSG_MSG_CODE( pMsg ) = AWMC_CLI_BASE;
                    SYS_MSG_FRAME_LEN( pMsg ) = 0;
                    SYS_MSG_BODY_STYLE( pMsg ) = MSG_BODY_INTEGRATIVE;
                    SYS_MSG_BODY_POINTER( pMsg ) = pCmd;

                    SYS_MSG_SRC_SLOT( pMsg ) = DEV_GetPhySlot();
                    SYS_MSG_DST_SLOT( pMsg ) = DEV_GetPhySlot();
                    /*pstMsg->usReserved              = usIfIndex;*/

                    /*add by jiangchao, 10/29 afternoon, for send message's question*/
                    pMsg->ulSequence = vty->g_ulSendMessageSequenceNo;



                    ulMsg[ 3 ] = ( ULONG ) pMsg;
                    vty_get( vty );
                    ulRet = VOS_QueSend( ( *( ULONG * ) pstCmdExecute->p_que_id ), ulMsg, VOS_WAIT_NO_WAIT, MSG_PRI_NORMAL );

                    if ( ulRet != VOS_OK )
                    {
                        vty_out( vty, "%% The corresponding module is very busy, can't deal with the command, please retry later.\r\n" );
                        VOS_Free( pMsg );
                        ret = CMD_ERR_NOTHING_TODO;
                        for ( i = 0; i < argc ; i++ )
                        {
                            if ( pCmd->argv[ i ] != NULL )
                            {
                                VOS_Free( pCmd->argv[ i ] );
                                pCmd->argv[ i ] = NULL ;
                            }
                        }
                        VOS_Free( pCmd );
                        pCmd = NULL ;
                        vty_out( vty, "%% Command failed, maybe corresponding module is not started.\r\n" );
                        vty_put( vty );
                        return ret;
                    }
                        /*add by jiangchao, 10/29 afternoon, for send message question*/
NextReceiveMessage:
                    /*在这里系统会收到两条消息，
                    第一条表明对方已经开始执行命令，第二条表明命令执行结束；
                    如果在 16秒(1000个tick) 内没有收到第一条消息，表明对方没有执行命令。
                    */

                    ulMsg[ 0 ] = 0;
                    ulMsg[ 1 ] = 0;
                    ulMsg[ 2 ] = 0;
                    ulMsg[ 3 ] = 0;

                    ulRet = VOS_QueReceive( vty->msg_que_id, ulMsg, gul_DelayNomalTicks );

                    if ( ulRet != VOS_HAVE_MSG )/*消息接收失败*/
                    {
                        if ( 0 == iRecvMsgSerial )  /*16秒内没有收到第一条消息*/
                        {
                            ret = CMD_ERR_NOTHING_TODO;                 

                            vty_out( vty, "%% CLI time out and command isn't executed, please retry later.\r\n" );

				/* modified by xieshl 20090620, 提升syslog处理命令行等级，以便写入nvram */
                            /*mn_syslog(LOG_TYPE_CLI_RECORD,LOG_INFO,"<%s><CLIREC><%s><%s><Didn't Executed Command [%s]>\r\n",mn_get_product_name(),mn_syslog_get_user_name(vty),mn_syslog_get_user_ip(vty),vty->buf);*/
				mn_syslog( LOG_TYPE_CLI_RECORD, 
						(SYS_MODULE_IS_RUNNING(SYS_LOCAL_MODULE_SLOTNO) ? LOG_ALERT : LOG_INFO),
                            		"%s %s %s :%s (Deny)", VOS_get_product_name(), mn_syslog_get_user_name(vty), mn_syslog_get_user_ip(vty),vty->buf);
                        }
                        else /*当前消息为第二条消息,接收失败*/
                        {
                            vty_out( vty, "%% CLI time out, but command will still be executed by the corresponding module, just a moment.\r\n" );
                        }                        
                        Cli_MessageSequenceIncrement( vty );
                        break;
                    }

                    /*以下为收到消息后的处理*/
                    pMsg = ( SYS_MSG_S* ) ulMsg[ 3 ];
                    if ( pMsg )
                    {
                        /*add by jiangchao, 10/29 afternoon, for send message question*/
                        if ( pMsg->ulSequence != vty->g_ulSendMessageSequenceNo )
                        {
                            VOS_Free( pMsg );  /*释放掉错误的消息*/
                            goto NextReceiveMessage;
                        }

                        if ( FIRST_MESSAGE_ID == ulMsg[ 2 ] )  /*接收到第一条消息*/
                        {
                            VOS_Free( pMsg );
                            iRecvMsgSerial++;
                            goto NextReceiveMessage; /*准备接收第二条消息*/
                        }

                        VOS_Free( pMsg );
                    }

                    /*add by jiangchao, 10/29 afternoon, for send message question*/
                    Cli_MessageSequenceIncrement( vty );

                }
                #endif
                break;

            case LDEFUN_FLAG:
                /* 在主控板上LDEFUN是向线路板发命令,在线路板上执行,
                但在线路板上,就是在本板执行,对于主控板和线路板都有的命令,
                现在并不支持使用DEFUN和LDEFUN注册两次*/
/*#ifndef __LIC__*//*commented by _DISTRIBUTE_PLATFORM_*/
/*            if(SYS_LOCAL_MODULE_ISMASTERACTIVE)
            	{
            RPU_SendCmd2LIC( pstCmdExecute->p_que_id, vty->buf, vty );
                ret = CMD_SUCCESS;
            	}
            else */
/*#else*//*commented by _DISTRIBUTE_PLATFORM_*/
            	{
             ret = ( *pstCmdExecute->func ) ( pstCmdExecute, vty, argc, argv );
            	}
/*#endif*/ /* #ifndef __LIC__ *//*commented by _DISTRIBUTE_PLATFORM_*/
                break;

                /* 为了执行时间长的命令使用 */
            case PDEFUN_FLAG:
                {
                    ULONG pdefun_old_taskPriority = TASK_PRIORITY_NORMAL; 
                    ULONG pdefun_new_taskPriority = TASK_PRIORITY_NORMAL;
                    LONG pdefun_res;
                    VOS_HANDLE pdefun_taskId;

                    pdefun_taskId = VOS_GetCurrentTask();
                    pdefun_res = VOS_TaskGetPriority( pdefun_taskId, &pdefun_old_taskPriority );
                    if ( pdefun_res == VOS_OK )
                    {
                        pdefun_new_taskPriority = TASK_PRIORITY_ABOVE_NORMAL;
                        VOS_TaskSetPriority( pdefun_taskId, pdefun_new_taskPriority );
                    }

                    /* 将命令行的优先级降低后，调用命令函数执行，这样不会丢心跳 */
                    ret = ( *pstCmdExecute->func ) ( pstCmdExecute, vty, argc, argv );
                    VOS_TaskSetPriority( pdefun_taskId, pdefun_old_taskPriority );
                }
                break;

            default:
                VOS_ASSERT(0);
                ret = CMD_ERR_NOTHING_TODO;
                break;
        }
    }
    
    if( CMD_SUCCESS == ret)
    {
        vty_cmd_hist_add_nvram(vty,pstCmdExecute,argc,argv);
        vty_cmd_vos_syslog(vty,pstCmdExecute,argc,argv);
    }
    else
    {
		#ifdef PLAT_MODULE_SYSLOG
		VOS_log (MODULE_RPU_CLI,
				(SYS_MODULE_IS_RUNNING(SYS_LOCAL_MODULE_SLOTNO_GET) ? SYSLOG_CRIT : SYSLOG_INFO),
				"%s %s %s :%s (Fail)", VOS_get_product_name(), vty_get_user_name(vty), vty_get_user_ip(vty), vty->buf);
		#endif
    }
    return ret;
}
/*-------------------------------------------------------
上面是解析规则的中间处理函数
********************************************************/


#if 1 
/**********************************************************
下面的函数主要进行上层的处理，和vty关系比较密切
----------------------------------------------------------*/


/*******************************************************
命令行的?查询、tab补齐、回车执行命令处理在下面的三个函数分别完成
-------------------------------------------------------*/ 

/***************************************************
获取?匹配的命令字段和可以执行的命令
输入参数 
输出参数 pstCmdExecute 指向可执行命令；NULL表明没有可执行函数。
              status 表明匹配状态(在这里只反馈三种状态)
              (no match or ambious match or match success)

*** 此函数是 ? 、回车、tab的基础函数。
---------------------------------------------------*/
cl_vector cmd_DescribeTabExecuteCmd ( cl_vector vline, struct vty * vty, LONG * status, struct cmd_element **pstCmdExecute )
{
    LONG index = 0;
    LONG lRet = 0;
    LONG lExecuteStatus = 0;
    LONG lExecuteCmdNum = 0;
    LONG lMatchCmdNum = 0; /*匹配了命令数量*/
    cl_vector cmd_cl_vector = NULL;
    cl_vector matchvec = NULL;
    struct cmd_element *pstCmd = NULL;

    *pstCmdExecute = NULL;
    *status = CMD_SUCCESS;
    if( ( NULL == vline ) || (NULL == vty) )
    {
        VOS_ASSERT(0);
        *status = CMD_ERR_NO_MATCH;
        return NULL;
    }
    if (cl_vector_max(vline) <= 0)
    {
        VOS_ASSERT(0);
        *status = CMD_ERR_NO_MATCH;
        return NULL ;
    }
    
    cmd_cl_vector = cl_vector_copy ( cmd_node_cl_vector ( cl_cmdvec_master, vty->node ) );
    if ( cmd_cl_vector == NULL ) /*申请了内存空间*/
    {
        VOS_ASSERT( 0 );
        *status = CMD_ERR_NO_MATCH;
        return NULL ;
    }

    vline->max--; /*vline->max肯定是>=1,可以--*/
    lRet = cmd_MatchCommands(vty, cmd_cl_vector, vline ); /*vline的最后一个片断不参与比较*/
    vline->max++;
    if ( -1 == lRet )
    {
        cl_vector_free( cmd_cl_vector );
        *status = CMD_ERR_AMBIGUOUS;
        return NULL;
    }

    if ( -2 == lRet ) /*in case the first keyword is ultra-long and unknown*/
    {
	cl_vector_free( cmd_cl_vector );
	*status = CMD_ERR_NO_MATCH;
	return NULL;
    }

    matchvec = cl_vector_init ( INIT_MATCHVEC_SIZE );
    if ( NULL == matchvec )
    {
        VOS_ASSERT( 0 );
        cl_vector_free( cmd_cl_vector );
        *status = CMD_ERR_NO_MATCH;
        return NULL;
    }

    /*匹配的命令集*/
    for ( index = 0; index < ( LONG ) cl_vector_max( cmd_cl_vector ); index++ )
    {
        pstCmd = cl_vector_slot( cmd_cl_vector, index );
        if ( NULL == pstCmd )
        {
            continue;
        }
        lMatchCmdNum++;
/*        mn_syslog( LOG_TYPE_CLI, LOG_DEBUG, "Match%d: %s\r\n", lMatchCmdNum, pstCmd->string );*/
        if ( cmd_entry_function_describe ( vty, pstCmd, vline, &lExecuteStatus, matchvec ) == 1 )  /*匹配*/
        {
            if ( 1 == lExecuteStatus ) /*此命令可以执行*/
            {
                if ( 0 == lExecuteCmdNum )
                {
                    *pstCmdExecute = pstCmd;
/*                    mn_syslog( LOG_TYPE_CLI, LOG_DEBUG, "This command can be executed\r\n" );*/
                }
                else if ( 1 <= lExecuteCmdNum )   /*存在多条可执行命令，因此命令不可执行*/
                {
                    VOS_ASSERT( 0 );
/*                    mn_syslog( LOG_TYPE_CLI, LOG_DEBUG, "Error: There is more than one command can be executed !" );*/
                    *pstCmdExecute = NULL;
                }
                lExecuteCmdNum++;
            }
        }
    }
    cl_vector_free( cmd_cl_vector );
/*    mn_syslog( LOG_TYPE_CLI, LOG_DEBUG, "Match cmd number: %d; Execute cmd number: %d\r\n", lMatchCmdNum, lExecuteCmdNum );*/

    if ( ( 0 == lMatchCmdNum ) || ( cl_vector_slot ( matchvec, 0 ) == NULL ) )  /*没有匹配的命令*/
    {
        if(0 < lMatchCmdNum) /*第二种unknown情况:输入命令的最后一个片断之前一直匹配命令，只有当前字段不匹配*/
        {
            cmd_ShowAmbiguousOrUnknow( vty,  vline, cl_vector_max(vline)-1, CMD_ERR_NO_MATCH);
        }
        cl_vector_free( matchvec );
        *status = CMD_ERR_NO_MATCH;
        return NULL;
    }
    VOS_qsort ( matchvec->index, matchvec->max, sizeof( void * ), cmp_desc );
    return matchvec;
}

/*************************************************
?查询帮助
-------------------------------------------------*/
cl_vector cmd_describe_command ( cl_vector vline, struct vty * vty, LONG * status )
{
    struct cmd_element *pstCmdExecute = NULL;
    cl_vector matchvec = NULL;
    if( (NULL == vline) || (NULL == vty) || (NULL == status) )
    {
        VOS_ASSERT(0);
        return NULL;
    }

    matchvec = cmd_DescribeTabExecuteCmd( vline, vty, status, &pstCmdExecute );
    return matchvec;
}

/************************************************
执行命令:按照"空格+?"来处理
------------------------------------------------*/

/** 
 * 命令执行
 * @param[in]   vline   用户输入命令
 * @param[in]   vty     当前会话
 * @param[in]   cmd     要执行的命令
 * @return      参看 cmd_ret_t
 */
int cmd_execute_command ( cl_vector vline, struct vty * vty, struct cmd_element **cmd )
{
    LONG status = CMD_SUCCESS;
    cl_vector matchvec = NULL;

    if( (NULL == vline) || (NULL == vty) || (NULL == cmd) )
    {
        VOS_ASSERT(0);
        return CMD_ERR_NO_MATCH;
    }

    if ( NULL != cl_vector_slot( vline, vline->max - 1 ) )  /*相当于"空格+?"*/
    {
        cl_vector_set( vline, NULL );
    }
    matchvec = cmd_DescribeTabExecuteCmd( vline, vty, &status, cmd );
    if ( NULL == cl_vector_slot( vline, vline->max - 1 ) )     /*恢复为原来的vline*/
    {
        cl_vector_unset( vline, vline->max - 1 );
    }

    if ( ( CMD_ERR_AMBIGUOUS == status ) ||
            ( CMD_ERR_NO_MATCH == status )
       )  /*此时matchvec为NULL*/
    {
        return status;
    }

    cl_vector_free( matchvec );

    if ( NULL == *cmd )  /*没有可以执行的命令,可见命令匹配不完整*/
    {
        return CMD_ERR_INCOMPLETE;
    }
    status = cmd_ExecuteOneCommand( vline, vty, *cmd );
    return /*CMD_SUCCESS*/status;
}

/************************************************
tab键补齐功能
------------------------------------------------*/
CHAR **cmd_complete_command ( cl_vector vline, struct vty * vty, LONG * status )
{

    cl_vector matchvec = NULL;
    cl_vector pstStrVec = NULL;
    struct cmd_element *pstCmdExecute = NULL;
    CHAR **pMatchStr = NULL;
    CHAR *pStrCommon = NULL;
    CHAR *pTmpStr = NULL;
    LONG index = 0;
    LONG lMatchStrLen = 0; /*匹配的多个关键字的相同部分长度*/
    struct desc *pDesc = NULL;
    
    if( (NULL == vline) || (NULL == vty) || (NULL == status) )
    {
        VOS_ASSERT(0);
        return NULL;
    }

    matchvec = cmd_DescribeTabExecuteCmd( vline, vty, status, &pstCmdExecute );
    if ( ( CMD_ERR_AMBIGUOUS == *status ) ||
            ( CMD_ERR_NO_MATCH == *status )
       )  /*此时matchvec为NULL*/
    {
        return NULL;
    }

    *status = CMD_ERR_NOTHING_TODO;
    
    pstStrVec = cl_vector_init(INIT_MATCHVEC_SIZE);
    if(NULL == pstStrVec)
    {
        cl_vector_free(matchvec);
        VOS_ASSERT(0);
        *status = CMD_ERR_NOTHING_TODO;
        return NULL;        
    }
    
    for(index = 0; index < (LONG) cl_vector_max(matchvec); index++)
    {
        pDesc = cl_vector_slot(matchvec,index);
        if(NULL == pDesc)
        {
            continue;
        }

        if(CMD_VARIABLE(pDesc->cmd)) /*变量不处理*/
        {
            continue;
        }

        pTmpStr = VOS_StrDup(pDesc->cmd, MODULE_RPU_CLI);
        if(NULL == pTmpStr)
        {
            cl_vector_free(matchvec);
            cmd_FreeStrVector(pstStrVec);            
            VOS_ASSERT(0);
            *status = CMD_ERR_NOTHING_TODO;
            return NULL; 
        }
        cl_vector_set(pstStrVec, pTmpStr);
    }
    cl_vector_set(pstStrVec, NULL); /*确保最后一个是NULL*/
    
    cl_vector_free(matchvec);/*matchvec已经完成它的任务了，释放掉内存*/
    matchvec = NULL;

    /*pstStrVec存储所有匹配的关键字*/    
    if(NULL == cl_vector_slot(pstStrVec, 0)) /*没有匹配到任何关键字,应该是匹配了变量*/
    {
        cl_vector_free(pstStrVec);
        *status = CMD_ERR_NOTHING_TODO;
        return NULL;
    }
        
    if(NULL == cl_vector_slot(pstStrVec, 1)) /*只匹配一个关键字*/
    {
        pMatchStr = ( CHAR ** ) pstStrVec->index;
        VOS_Free(pstStrVec); /*只释放节点，不释放索引*/
        *status = CMD_COMPLETE_FULL_MATCH;
        return pMatchStr;        
    }
    
    /*下面是匹配了多个关键字*/
    if ( cl_vector_slot ( vline, vline->max-1 ) != NULL ) /*不是 空格+tab*/
    {
        lMatchStrLen = cmd_lcd ( ( CHAR ** ) pstStrVec->index );

        if ( lMatchStrLen )
        {
            LONG len = VOS_StrLen ( cl_vector_slot ( vline, vline->max-1 ) );

            if ( len < lMatchStrLen )
            {
               
                pStrCommon = ( CHAR * ) VOS_Malloc( ( lMatchStrLen + 1 ), MODULE_RPU_CLI );
                if ( pStrCommon == NULL )
                {
                    cmd_FreeStrVector(pstStrVec);
                    VOS_ASSERT( 0 );
                    *status = CMD_ERR_NOTHING_TODO;
                    return NULL;
                }

                VOS_StrnCpy( pStrCommon, pstStrVec->index[ 0 ], lMatchStrLen );
                pStrCommon[ lMatchStrLen ] = '\0';

                cmd_FreeStrVector(pstStrVec);
                
                pstStrVec = cl_vector_init ( INIT_MATCHVEC_SIZE );
                if ( NULL == pstStrVec )
                {
                    VOS_ASSERT( 0 );
                    *status = CMD_ERR_NOTHING_TODO;
                    return NULL;
                }

                cl_vector_set ( pstStrVec, pStrCommon );
                pMatchStr = ( CHAR ** ) pstStrVec->index;
                VOS_Free ( pstStrVec );

                *status = CMD_COMPLETE_MATCH;
                return pMatchStr;
            }
        }
    }

    pMatchStr = ( CHAR ** ) pstStrVec->index;
    VOS_Free ( pstStrVec );
    *status = CMD_COMPLETE_LIST_MATCH;
    return pMatchStr;
}

/*-------------------------------------------------------
上面的函数主要进行上层的处理，和vty关系比较密切
*********************************************************/
#endif

#ifdef __cplusplus
}
#endif

