/**********************************************************************************************************************\
*  文件： fastq.h
*  介绍： 低时延队列
*  作者： 荣涛
*  日期：
*       2021年1月25日    创建与开发轮询功能
*       2021年1月27日 添加 通知+轮询 功能接口，消除零消息时的 CPU100% 问题
*       2021年1月28日 调整代码格式，添加必要的注释
*       2021年2月1日 添加多入单出队列功能
*       2021年2月2日 消息队列句柄改成 unsigned long 索引
\**********************************************************************************************************************/

#ifndef __fAStMQ_H
#define __fAStMQ_H 1

#include <stdbool.h>

#ifndef always_inline
#define always_inline __attribute__ ((__always_inline__))
#endif


#ifdef MODULE_ID_MAX //VOS moduleID 最大模块索引值
#define FASTQ_ID_MAX    MODULE_ID_MAX
#else
#define FASTQ_ID_MAX    256
#endif

/**
 *  内存分配器接口
 */
#define FastQMalloc(size) malloc(size)
#define FastQFree(ptr) free(ptr)

/**
 *  源模块未初始化时可临时使用的模块ID，只允许使用一次 
 */
#define FastQTmpModuleID    0 

/**
 *  msg_handler_t - FastQRecvMain 接收函数
 *  
 *  param[in]   msg 接收消息地址
 *  param[in]   sz 接收消息大小，与 FastQCreate (..., msg_size) 保持一致
 */
typedef void (*msg_handler_t)(void*msg, size_t sz);

/**
 *  FastQCreateModule - 注册消息队列
 *  
 *  param[in]   moduleID    模块ID， 范围 1 - FASTQ_ID_MAX
 *  param[in]   msgMax      该模块 的 消息队列 的大小
 *  param[in]   msgSize     最大传递的消息大小
 */
always_inline void 
FastQCreateModule(const unsigned long moduleID, const unsigned int msgMax, const unsigned int msgSize);

/**
 *  FastQDump - 显示信息
 *  
 *  param[in]   fp    文件指针
 */
always_inline void 
FastQDump(FILE*fp);

/**
 *  FastQSend - 发送消息（轮询直至成功发送）
 *  
 *  param[in]   from    源模块ID， 范围 1 - FASTQ_ID_MAX 
 *  param[in]   to      目的模块ID， 范围 1 - FASTQ_ID_MAX
 *  param[in]   msg     传递的消息体
 *  param[in]   size    传递的消息大小
 *
 *  return 成功true （轮询直至发送成功，只可能返回 true ）
 *
 *  注意：from 和 to 需要使用 FastQCreateModule 注册后使用
 */
always_inline bool 
FastQSend(unsigned int from, unsigned int to, const void *msg, size_t size);


/**
 *  FastQTrySend - 发送消息（尝试向队列中插入，当队列满是直接返回false）
 *  
 *  param[in]   from    源模块ID， 范围 1 - FASTQ_ID_MAX 
 *  param[in]   to      目的模块ID， 范围 1 - FASTQ_ID_MAX
 *  param[in]   msg     传递的消息体
 *  param[in]   size    传递的消息大小
 *
 *  return 成功true 失败false
 *
 *  注意：from 和 to 需要使用 FastQCreateModule 注册后使用
 */
always_inline bool 
FastQTrySend(unsigned int from, unsigned int to, const void *msg, size_t size);

/**
 *  FastQRecv - 接收消息
 *  
 *  param[in]   from    从模块ID from 中读取消息， 范围 1 - FASTQ_ID_MAX 
 *  param[in]   handler 消息处理函数，参照 msg_handler_t 说明
 *
 *  return 成功true 失败false
 *
 *  注意：from 需要使用 FastQCreateModule 注册后使用
 */
always_inline  bool 
FastQRecv(unsigned int from, msg_handler_t handler);


#endif /*<__fAStMQ_H>*/

