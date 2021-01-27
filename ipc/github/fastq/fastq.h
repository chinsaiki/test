/**********************************************************************************************************************\
*  文件： fastq.h
*  介绍： 低时延队列
*  作者： 荣涛
*  日期：
*       2021年1月25日    创建与开发轮询功能
*       2021年1月27日 添加 通知+轮询 功能接口，消除零消息时的 CPU100% 问题
\**********************************************************************************************************************/

#ifndef __fAStMQ_H
#define __fAStMQ_H 1

#include <stdbool.h>

// Forced inlining 
#ifndef always_inline
#define always_inline __attribute__ ((__always_inline__))
#endif

/**
 * FastQ 接口概览
 *
 *	fastq_create        初始化 FastQ 句柄 fastq_context    
 *	fastq_ctx_print     Dump FastQ 信息
 *
 *	fastq_sendto        发送接口：轮询方式（直至发送成功为止返回）
 *	fastq_sendnb        发送接口：非阻塞方式
 *	fastq_recvfrom      接收接口：轮询方式（直至接收成功为止返回）
 *	fastq_recvnb        接收接口：非阻塞方式
 *
 *	fastq_sendto_main   发送接口：轮询发送+通知（直至发送成功为止返回）
 *  fastq_sendtry_main  发送接口：尝试发送+通知（可能由于队列满等问题发送失败）
 *	fastq_recv_main     接收接口：接收通知+轮询（直至接收完队列中所有消息）
 *  
 *  具体使用请参见文件末尾的实例代码
 */

/**
 *  不需要关注的结构
 */
struct fastq_header;
struct fastq_ring;

/**
 *  fastq_context - fastq 队列上下文
 *  
 *  fastq_context 结构中的字段不允许用户修改
 *  
 *  param[*]    p_  根据 fastq_create 参数申请的内存空间
 *  param[*]    header_ 保存参数信息
 *  param[*]    ring_ 轮询ring
 *  param[*]    data_ ring结构数据地址
 *  param[*]    irq 接收接口的中断描述符， 对 fastq_recv_main 生效
 */
typedef struct fastq_context {
    struct fastq_header *hdr;
    struct fastq_ring *ring;
    void *ptr;   
    char *data;
#define FASTQ_MAX_NODE  4
    int irq[FASTQ_MAX_NODE];
};

/**
 *  msg_handler_t - fastq_recv_main 接收函数
 *  
 *  param[in]   msg 接收消息地址
 *  param[in]   sz 接收消息大小，与 fastq_create (..., msg_size) 保持一致
 */
typedef void (*msg_handler_t)(void*msg, size_t sz);


/**
 *  fastq_create - fastq_context 实体句柄的初始化
 *  
 *  param[in]   self 见 fastq_context
 *  param[in]   nodes 使用 fastq_context 交互的线程数，参见 发送和接收 API 的 from 和 to 字段
 *                  大小不可大于 FASTQ_MAX_NODE
 *  param[in]   size ring队列中的节点数
 *  param[in]   msg_size ring队列中的节点大小，若传递指针，填写 sizeof(unsigned long)
 *
 *  return 成功 返回 true ， 失败 返回 false
 */
always_inline bool 
fastq_create(struct fastq_context *self, unsigned int nodes, unsigned int size, unsigned int msg_size);


/**
 *  fastq_ctx_print - fastq_context 信息 dump
 *  
 *  param[in]   fp 文件指针
 *  param[in]   self 见 fastq_context
 *
 *  return 成功 返回 true ， 失败 返回 false
 */
always_inline void 
fastq_ctx_print(FILE*fp, struct fastq_context *self);


/**
 *  fastq_sendto - 轮询方式发送消息（该接口将占满 CPU-100%）
 *  
 *  注意： *** 使用该接口请查看示例代码 ***
 *  
 *  param[in]   self 见 fastq_context
 *  param[in]   from 发送使用 的 ring， 参见 fastq_create 的 nodes 参数
 *  param[in]   to 向 to ring 发送， 参见 fastq_create 的 nodes 参数
 *  param[in]   msg 消息体指针
 *  param[in]   size ring队列中的节点大小，若传递指针，填写 sizeof(unsigned long)
 *
 *  return 成功 返回 true ， 失败 返回 false
 */
always_inline bool 
fastq_sendto(struct fastq_context *self, unsigned int from, unsigned int to, const void *msg, size_t size);



/**
 *  fastq_sendnb - 非阻塞方式发送消息
 *  
 *  注意： *** 注意判断返回值 ***
 *  
 *  param[in]   self 见 fastq_context
 *  param[in]   from 发送使用 的 ring， 参见 fastq_create 的 nodes 参数
 *  param[in]   to 向 to ring 发送， 参见 fastq_create 的 nodes 参数
 *  param[in]   msg 消息体指针
 *  param[in]   size ring队列中的节点大小，若传递指针，填写 sizeof(unsigned long)
 *
 *  return 成功 返回 true ， 失败 返回 false
 */
always_inline bool
fastq_sendnb(struct fastq_context *self, unsigned int from, unsigned int to, const void *msg, size_t size);



/**
 *  fastq_recvfrom - 轮询方式接收消息（该接口将占满 CPU-100%）
 *  
 *  注意： *** 使用该接口请查看示例代码 ***
 *  
 *  param[in]   self 见 fastq_context
 *  param[in]   from 发送使用 的 ring， 参见 fastq_create 的 nodes 参数
 *  param[in]   to 向 to ring 发送， 参见 fastq_create 的 nodes 参数
 *  param[in]   msg 消息体指针
 *  param[in]   size ring队列中的节点大小，若传递指针，填写 sizeof(unsigned long)
 *
 *  return 成功 返回 true ， 失败 返回 false
 */
always_inline bool 
fastq_recvfrom(struct fastq_context *self, unsigned int from, unsigned int to, void *msg, size_t *size);



/**
 *  fastq_recvnb - 非阻塞方式接收消息
 *  
 *  注意： *** 注意判断返回值 ***
 *  
 *  param[in]   self 见 fastq_context
 *  param[in]   from 发送使用 的 ring， 参见 fastq_create 的 nodes 参数
 *  param[in]   to 向 to ring 发送， 参见 fastq_create 的 nodes 参数
 *  param[in]   msg 消息体指针
 *  param[in]   size ring队列中的节点大小，若传递指针，填写 sizeof(unsigned long)
 *
 *  return 成功 返回 true ， 失败 返回 false
 */
always_inline bool 
fastq_recvnb(struct fastq_context *self, unsigned int from, unsigned int to, void *msg, size_t *size);



/**
 *  fastq_sendto_main - 通知 + 轮询方式发送消息
 *  
 *  接口步骤：
 *      1. 轮询入队
 *      2. 通知
 *  
 *  注意： *** 推荐使用该接口 *** 必须使用 fastq_recv_main 接口 接收
 *  
 *  param[in]   self 见 fastq_context
 *  param[in]   from 发送使用 的 ring， 参见 fastq_create 的 nodes 参数
 *  param[in]   to 向 to ring 发送， 参见 fastq_create 的 nodes 参数
 *  param[in]   msg 消息体指针
 *  param[in]   size ring队列中的节点大小，若传递指针，填写 sizeof(unsigned long)
 *
 *  return 成功 返回 true ， 失败 返回 false
 */
always_inline bool 
fastq_sendto_main(struct fastq_context *self, unsigned int from, unsigned int to, const void *msg, size_t size);


/**
 *  fastq_sendtry_main - 通知 + 尝试方式发送消息（可能由于队列满发送失败）
 *  
 *  接口步骤：
 *      1. 轮询入队
 *      2. 通知
 *  
 *  注意： *** 推荐使用该接口 *** 必须使用 fastq_recv_main 接口 接收
 *  
 *  param[in]   self 见 fastq_context
 *  param[in]   from 发送使用 的 ring， 参见 fastq_create 的 nodes 参数
 *  param[in]   to 向 to ring 发送， 参见 fastq_create 的 nodes 参数
 *  param[in]   msg 消息体指针
 *  param[in]   size ring队列中的节点大小，若传递指针，填写 sizeof(unsigned long)
 *
 *  return 成功 返回 true ， 失败 返回 false
 */
always_inline bool 
fastq_sendtry_main(struct fastq_context *self, unsigned int from, unsigned int to, const void *msg, size_t size);


/**
 *  fastq_recv_main - 通知 + 轮询方式接收消息
 *  
 *  接口步骤：
 *      1. 等待通知
 *      2. 轮询出队
 *  
 *  注意： *** 推荐使用该接口 *** 必须使用 fastq_sendto_main 接口 发送
 *  
 *  param[in]   self 见 fastq_context
 *  param[in]   from 发送使用 的 ring， 参见 fastq_create 的 nodes 参数
 *  param[in]   to 向 to ring 发送， 参见 fastq_create 的 nodes 参数
 *  param[in]   msg 消息体指针
 *  param[in]   size ring队列中的节点大小，若传递指针，填写 sizeof(unsigned long)
 *
 *  return 成功 返回 true ， 失败 返回 false
 */
always_inline  bool 
fastq_recv_main(struct fastq_context *self, unsigned int from, unsigned int to, msg_handler_t handler);


#endif /*<__fAStMQ_H>*/


/**********************************************************************************************************************\
 *
 *
 *  以下为示例程序, 该部分代码不允许被编译
 *
 *
\**********************************************************************************************************************/

#ifdef __do_not_define_this_FastQ_TEST
# error You gotta be kidding me, do not define __do_not_define_this_FastQ_TEST
#endif
#ifdef __do_not_define_this_FastQ_TEST

/**********************************************************************************************************************\
 *
 *  示例1： 传递一个 无符号长整形/地址（unsigned long）的轮询示例程序
 *  
\**********************************************************************************************************************/
#define TEST_NUM   (1UL<<20)

#define NODE_0 0
#define NODE_1 1
#define NODE_NR 2

typedef struct  {
#define TEST_MSG_MAGIC 0x123123ff    
    int magic;
    unsigned long value;
}__attribute__((aligned(64))) test_msgs_t;

test_msgs_t *test_msgs;
struct fastq_context fastq_ctx;


void *test_1_enqueue_task(void*arg){
    struct fastq_context *ctx = (struct fastq_context *)arg;
    int i =0;
    test_msgs_t *pmsg;
    while(1) {
        pmsg = &test_msgs[i++%TEST_NUM];
        pmsg->latency = RDTSC();
        unsigned long addr = (unsigned long)pmsg;
        fastq_sendto(ctx, NODE_0, NODE_1, &addr, sizeof(unsigned long));
    }
    pthread_exit(NULL);
}

void *test_1_dequeue_task(void*arg) {
    struct fastq_context *ctx = (struct fastq_context *)arg;

    size_t sz = sizeof(unsigned long);
    test_msgs_t *pmsg;
    unsigned long addr;
    while(1) {
        fastq_recvfrom(ctx, NODE_0, NODE_1, &addr, &sz);
        pmsg = (test_msgs_t *)addr;
        
        /* 处理流程 */
    }
    pthread_exit(NULL);
}

int main()
{
    ...
        
    fastq_create(&fastq_ctx, NODE_NR, 8, sizeof(unsigned long));
    
    unsigned int i =0;
    test_msgs = (test_msgs_t *)malloc(sizeof(test_msgs_t)*TEST_NUM);
    for(i=0;i<TEST_NUM;i++) {
        test_msgs[i].magic = TEST_MSG_MAGIC; //检查错误传输的消息
        test_msgs[i].value = i+1;
    }


    pthread_create(..., test_1_enqueue_task, &fastq_ctx);
    pthread_create(..., test_1_dequeue_task, &fastq_ctx);

    ...
}


/**********************************************************************************************************************\
 *
 *  示例2： 传递一个 无符号长整形/地址（unsigned long）的通知+轮询示例程序
 *
 *  这里和上面冗余部分的代码不在给出，只给出关键的 发送 和 接收 线程的回调函数
 *  
\**********************************************************************************************************************/

void *test_2_enqueue_task(void*arg){
    struct fastq_context *ctx = (struct fastq_context *)arg;
    int i =0;
    test_msgs_t *pmsg = NULL;
    
    while(1) {
        pmsg = &test_msgs[i++%TEST_NUM];
        pmsg->latency = RDTSC();
        unsigned long addr = (unsigned long)pmsg;
        fastq_sendto_main(ctx, NODE_0, NODE_1, &addr, sizeof(unsigned long));
    }
    pthread_exit(NULL);
}

void handle_test_msg(void* msg, size_t size)
{
    test_msgs_t *pmsg = (test_msgs_t *)msg;

    /* 处理流程 */
}

void *test_2_dequeue_task(void*arg) {
    struct fastq_context *ctx = (struct fastq_context *)arg;

    fastq_recv_main(ctx, MODULE_1, MODULE_2, handle_test_msg);
    
    pthread_exit(NULL);
}

/**********************************************************************************************************************\
 *
 *  示例程序结束
 *
\**********************************************************************************************************************/
#endif /*<测试代码结束>*/


