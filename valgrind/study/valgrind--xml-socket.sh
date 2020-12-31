#!/bin/bash

# --leak-check=full 完全检查内存泄漏
# --show-reachable=yes 显示内存泄漏地点
# --trace-children=yes 跟入子进程
# --quiet 只打印错误信息

if [ $# -lt 1 ]; then
	echo "usage: $0 [program]"
	exit
fi

#开启监听  nc -l 10.170.6.66 1500
# valgrind-listener 默认为 1500 端口
#	或者使用 valgrind-listener 1234 修改端口

valgrind --leak-check=full \
		 --show-reachable=yes \
		 --trace-children=yes \
		 --time-stamp=yes \
		 --xml=yes \
		 --xml-socket=10.170.6.66:1500 \
		 --quiet \
		 $*
