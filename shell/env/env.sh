#!/bin/bash
# 荣涛 2021年2月22日

#HOME: 使用者的目录地址
echo $HOME
#BASH: 目前使用的shell
echo $BASH
#HISTSIZE: 保留历史命令的数目上限
echo $HISTSIZE
#PATH：系统默认的可执行文件搜索路径
echo $PATH
#LANG：系统使用的编码。很多程序是根据系统指定的编码去解析输入文件，如果编码不正确，很多文件就无法正确解析。
echo $LANG
#RANDOM：随机产生一个0~32767之间的随机数
echo $RANDOM
#PS1：即命令提示符的格式，就是shell命令行前方的图标。
echo $PS1
#OSTYPE：系统类型
echo $OSTYPE
#HOSTTYPE, MACHTYPE：系统指令集的位数（例如：x86_64）
echo $HOSTTYPE
echo $MACHTYPE

set TEST_RT=/home/rongtao
