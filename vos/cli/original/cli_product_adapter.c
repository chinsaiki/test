/**
 * @file cli_product_adapter.c
 * @brief cli 适配
 * @details 产品需要注册给命令行的相关函数
 * @version 1.0
 * @author  wujianming  (wujianming@sylincom.com)
 * @date 2019.01.25
 * @copyright 
 *    Copyright (c) 2018 Sylincom.
 *    All rights reserved.
*/


#ifdef __cplusplus
extern"C"{
#endif

#include "vos_global.h"
#include "vos_common.h"
#include "cli/cl_set.h"
#include "plat_adaptor.h"
#include "cli/cli.h"

LONG (*pfunc_vos_get_host_name)(CHAR *,INT);
LONG (*pfunc_vos_get_product_name)(CHAR *,INT );
LONG (*pfunc_vos_get_cli_welcomeStr)(CHAR *,INT );


CHAR vos_hostname[VOS_SYSNAME_LEN]=_CL_DEFAULT_HOST_NAME_;

CHAR vos_product_name[VOS_PRODUCT_NAME_LEN]="VOS_product";

#define VOS_CLI_WELCOME__LEN (512)
CHAR vos_cli_welcomeStr[VOS_CLI_WELCOME__LEN];


CHAR *VOS_get_hostname(VOID)
{
#ifdef PLAT_MODULE_ADAPTOR
    if(CDSMS_PRODUCT_NAME_FUN)
    {
        CDSMS_PRODUCT_NAME_FUN((UCHAR *)vos_hostname,VOS_SYSNAME_LEN);
    }
#endif

    return vos_hostname;
}


CHAR *VOS_get_product_name(VOID)
{
#ifdef PLAT_MODULE_ADAPTOR
    if(CDSMS_PRODUCT_NAME_FUN)
    {
        CDSMS_PRODUCT_NAME_FUN((UCHAR *)vos_product_name,VOS_PRODUCT_NAME_LEN);
    }
#endif

    return vos_product_name;
}


CHAR *VOS_get_cli_welcomeStr(VOID)
{
    if(pfunc_vos_get_cli_welcomeStr)
    {
        pfunc_vos_get_cli_welcomeStr(vos_cli_welcomeStr,VOS_CLI_WELCOME__LEN);
    }
    else
    {
        ULONG len = 0;
        len += VOS_Sprintf(&vos_cli_welcomeStr[len],"############################################################\r\n");
        len += VOS_Sprintf(&vos_cli_welcomeStr[len],"#                                                          #\r\n");
        len += VOS_Sprintf(&vos_cli_welcomeStr[len],"#               Welcome to %-32s#\r\n",VOS_get_product_name());
        len += VOS_Sprintf(&vos_cli_welcomeStr[len],"#                                                          #\r\n");
        len += VOS_Sprintf(&vos_cli_welcomeStr[len],"############################################################\r\n");

    }

    return vos_cli_welcomeStr;
}



#ifdef __cplusplus
}
#endif

