/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 06月 14日 星期五 09:05:24 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 04月 02日 星期二 08:54:32 CST. */


/**
 * NAME
 * setbuf, setbuffer, setlinebuf, setvbuf - 流缓冲操作
 *
 * SYNOPSIS 总览 
 */
#include <stdio.h>

void setbuf(FILE *stream, char *buf);
void setbuffer(FILE *stream, char *buf, size_t size);

//它们是无缓冲，块缓冲和行缓冲。
void setlinebuf(FILE *stream);//行缓冲。
int setvbuf(FILE *stream, char *buf, int mode , size_t size);

//函数 setvbuf 可以用在任何打开的流上，改变它的缓冲。参数 mode 必须是下列三个宏之一：
//  _IONBF 无缓冲
//  _IOLBF 行缓冲
//  _IOFBF 完全缓冲



/**
 * DESCRIPTION 描述
 * 有三种类型的缓冲策略，它们是无缓冲，块缓冲和行缓冲。当输出流无缓冲时，信息在写的同时出现于目标文
 * 件或终端上；当是块缓冲时，字符被暂存，然后一起写入；当是行缓冲时，字符被暂存，直到要输出一个新行符，或者从任何与终端设备连接的流中
 * (典型的是  stdin)  读取输入时才输出。函数  fflush(3)  可以用来强制提前输出。(参见  fclose(3))
 * 通常所有文件都是块缓冲的。当文件        I/O        操作在文件上发生时，将调用       malloc(3)
 * ，获得一个缓冲。如果流指向一个终端   (通常   stdout   都是这样)，那么它是行缓冲的。标准错误流
 * stderr 默认总 是无缓冲的。
 */
