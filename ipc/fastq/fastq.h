/**********************************************************************************************************************\
*  文件： fastq.h
*  介绍： 低时延队列
*  作者： 荣涛
*  日期：
*       2021年1月25日    创建与开发轮询功能
*       2021年1月27日 添加 通知+轮询 功能接口，消除零消息时的 CPU100% 问题
*       2021年1月28日 调整代码格式，添加必要的注释
*       2021年2月1日 添加多入单出队列功能
\**********************************************************************************************************************/

#ifndef __fAStMQ_H
#define __fAStMQ_H 1

#include <stdbool.h>

/* 强制inline，尽可能减少执行的指令数
    若需要调试，请编译时 定义 */
#ifndef always_inline
#define always_inline __attribute__ ((__always_inline__))
#endif

/**
 * FastQ 接口概览
 *
 *	FastQCreate         初始化 FastQ 句柄 FastQContext    
 *	FastQCtxPrint       Dump FastQ 信息
 *
 *	FastQSendto         发送接口：轮询方式（直至发送成功为止返回）
 *	FastQSendNonblk     发送接口：非阻塞方式
 *	FastQRecvrom        接收接口：轮询方式（直至接收成功为止返回）
 *	FastQRecvNonblk     接收接口：非阻塞方式
 *
 *	FastQSendMain       发送接口：轮询发送+通知（直至发送成功为止返回）
 *  FastQTrySendMain    发送接口：尝试发送+通知（可能由于队列满等问题发送失败）
 *	FastQRecvMain       接收接口：接收通知+轮询（直至接收完队列中所有消息）
 *  
 *  具体使用请参见文件末尾的实例代码
 */

/**
 *  不需要关注的结构
 */
struct FastQHeader;
struct FastQRing;

/**
 *  FastQContext - fastq 队列上下文
 *  
 *  FastQContext 结构中的字段不允许用户修改
 *  
 *  param[*]    p  根据 FastQCreate 参数申请的内存空间
 *  param[*]    header 保存参数信息
 *  param[*]    ring 轮询ring
 *  param[*]    pepfd 监听所有接收队列的epoll fd
 *  param[*]    data ring结构数据地址
 */
struct FastQContext {
    struct FastQHeader *hdr;
    struct FastQRing *ring;
    int *pepfd;
    void *ptr;   
    char *data;
};

/**
 *  msg_handler_t - FastQRecvMain 接收函数
 *  
 *  param[in]   msg 接收消息地址
 *  param[in]   sz 接收消息大小，与 FastQCreate (..., msg_size) 保持一致
 */
typedef void (*msg_handler_t)(void*msg, size_t sz);


/**
 *  FastQCreate - FastQContext 实体句柄的初始化
 *  
 *  param[in]   self 见 FastQContext
 *  param[in]   nodes 使用 FastQContext 交互的线程数，参见 发送和接收 API 的 from 和 to 字段
 *                  大小不可大于 FASTQ_MAX_NODE
 *  param[in]   size ring队列中的节点数
 *  param[in]   msg_size ring队列中的节点大小，若传递指针，填写 sizeof(unsigned long)
 *
 *  return 成功 返回 true ， 失败 返回 false
 */
always_inline bool 
FastQCreate(struct FastQContext *self, unsigned int nodes, unsigned int size, unsigned int msg_size);


/**
 *  FastQCtxPrint - FastQContext 信息 dump
 *  
 *  param[in]   fp 文件指针
 *  param[in]   self 见 FastQContext
 *
 *  return 成功 返回 true ， 失败 返回 false
 */
always_inline void 
FastQCtxPrint(FILE*fp, struct FastQContext *self);


/**
 *  FastQSendto - 轮询方式发送消息（该接口将占满 CPU-100%）
 *  
 *  注意： *** 使用该接口请查看示例代码 ***
 *  
 *  param[in]   self 见 FastQContext
 *  param[in]   from 发送使用 的 ring， 参见 FastQCreate 的 nodes 参数
 *  param[in]   to 向 to ring 发送， 参见 FastQCreate 的 nodes 参数
 *  param[in]   msg 消息体指针
 *  param[in]   size ring队列中的节点大小，若传递指针，填写 sizeof(unsigned long)
 *
 *  return 成功 返回 true ， 失败 返回 false
 */
always_inline bool 
FastQSendto(struct FastQContext *self, unsigned int from, unsigned int to, const void *msg, size_t size);



/**
 *  FastQSendNonblk - 非阻塞方式发送消息
 *  
 *  注意： *** 注意判断返回值 ***
 *  
 *  param[in]   self 见 FastQContext
 *  param[in]   from 发送使用 的 ring， 参见 FastQCreate 的 nodes 参数
 *  param[in]   to 向 to ring 发送， 参见 FastQCreate 的 nodes 参数
 *  param[in]   msg 消息体指针
 *  param[in]   size ring队列中的节点大小，若传递指针，填写 sizeof(unsigned long)
 *
 *  return 成功 返回 true ， 失败 返回 false
 */
always_inline bool
FastQSendNonblk(struct FastQContext *self, unsigned int from, unsigned int to, const void *msg, size_t size);



/**
 *  FastQRecvrom - 轮询方式接收消息（该接口将占满 CPU-100%）
 *  
 *  注意： *** 使用该接口请查看示例代码 ***
 *  
 *  param[in]   self 见 FastQContext
 *  param[in]   from 发送使用 的 ring， 参见 FastQCreate 的 nodes 参数
 *  param[in]   to 向 to ring 发送， 参见 FastQCreate 的 nodes 参数
 *  param[in]   msg 消息体指针
 *  param[in]   size ring队列中的节点大小，若传递指针，填写 sizeof(unsigned long)
 *
 *  return 成功 返回 true ， 失败 返回 false
 */
always_inline bool 
FastQRecvrom(struct FastQContext *self, unsigned int from, unsigned int to, void *msg, size_t *size);



/**
 *  FastQRecvNonblk - 非阻塞方式接收消息
 *  
 *  注意： *** 注意判断返回值 ***
 *  
 *  param[in]   self 见 FastQContext
 *  param[in]   from 发送使用 的 ring， 参见 FastQCreate 的 nodes 参数
 *  param[in]   to 向 to ring 发送， 参见 FastQCreate 的 nodes 参数
 *  param[in]   msg 消息体指针
 *  param[in]   size ring队列中的节点大小，若传递指针，填写 sizeof(unsigned long)
 *
 *  return 成功 返回 true ， 失败 返回 false
 */
always_inline bool 
FastQRecvNonblk(struct FastQContext *self, unsigned int from, unsigned int to, void *msg, size_t *size);



/**
 *  FastQSendMain - 通知 + 轮询方式发送消息
 *  
 *  接口步骤：
 *      1. 轮询入队
 *      2. 通知
 *  
 *  注意： *** 推荐使用该接口 *** 必须使用 FastQRecvMain 接口 接收
 *  
 *  param[in]   self 见 FastQContext
 *  param[in]   from 发送使用 的 ring， 参见 FastQCreate 的 nodes 参数
 *  param[in]   to 向 to ring 发送， 参见 FastQCreate 的 nodes 参数
 *  param[in]   msg 消息体指针
 *  param[in]   size ring队列中的节点大小，若传递指针，填写 sizeof(unsigned long)
 *
 *  return 成功 返回 true ， 失败 返回 false
 */
always_inline bool 
FastQSendMain(struct FastQContext *self, unsigned int from, unsigned int to, const void *msg, size_t size);


/**
 *  FastQTrySendMain - 通知 + 尝试方式发送消息（可能由于队列满发送失败）
 *  
 *  接口步骤：
 *      1. 轮询入队
 *      2. 通知
 *  
 *  注意： *** 推荐使用该接口 *** 必须使用 FastQRecvMain 接口 接收
 *  
 *  param[in]   self 见 FastQContext
 *  param[in]   from 发送使用 的 ring， 参见 FastQCreate 的 nodes 参数
 *  param[in]   to 向 to ring 发送， 参见 FastQCreate 的 nodes 参数
 *  param[in]   msg 消息体指针
 *  param[in]   size ring队列中的节点大小，若传递指针，填写 sizeof(unsigned long)
 *
 *  return 成功 返回 true ， 失败 返回 false
 */
always_inline bool 
FastQTrySendMain(struct FastQContext *self, unsigned int from, unsigned int to, const void *msg, size_t size);


/**
 *  FastQRecvMain - 通知 + 轮询方式接收消息
 *  
 *  接口步骤：
 *      1. 等待通知
 *      2. 轮询出队
 *  
 *  注意： *** 推荐使用该接口 *** 必须使用 FastQSendMain 接口 发送
 *  
 *  param[in]   self 见 FastQContext
 *  param[in]   node 接收该 node 上 所有环形队列的消息， 参见 FastQCreate 的 nodes 参数
 *  param[in]   msg 消息体指针
 *  param[in]   size ring队列中的节点大小，若传递指针，填写 sizeof(unsigned long)
 *
 *  return 成功 返回 true ， 失败 返回 false
 */
always_inline  bool 
FastQRecvMain(struct FastQContext *self, unsigned int node, msg_handler_t handler);


#endif /*<__fAStMQ_H>*/

