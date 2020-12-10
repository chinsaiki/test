/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

/**
 *	sctp_sendmsg
 *
 *	使用伴随辅助数据的sendmsg函数，应用进程能够控制sctp的各种特性，
 *	然而既然是用辅助数据可能不太方便，许多sctp实现提供了一个辅助函数库调用
 *	（有可能作为系统调用实现），以方便应用进程使用sctp的高级特性。
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/sctp.h>

ssize_t sctp_sendmsg(int sockfd, const void *msg, size_t msgsz,
					const struct sockaddr *to, socklen_t tolen,
					uint32_t ppid, uint32_t flags, uint16_t stream,
					uint32_t timetolive, uint32_t context);

/**
 *	sockfd:
 *	msg：长度为msgsz字节的缓冲区
 *	msgsz：msg字节数
 *	tolen：是定存放在to中的地址长度
 *	ppid：指定将随数据块传递的净荷协议标识符。
 *	flags：传递给SCTP栈，用于标识任何SCTP选项
 *	stream：调用者在stream参数中指定一个sctp流号。
 *	timetolive：以毫秒问单位指定消息的生命期，其中0表示无线生命期
 *	context：指定可能有的用户上下文
 *
 *	例子：
 *		ret = sctp_sendmsg(sockfd, data, datasz, &dest, sizeof(dest),
 *							24, MSG_PR_SCTP_TTL, 1 ,1000, 52);
 */

