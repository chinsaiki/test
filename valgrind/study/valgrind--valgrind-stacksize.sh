#!/bin/bash

# --leak-check=full 完全检查内存泄漏
# --show-reachable=yes 显示内存泄漏地点
# --trace-children=yes 跟入子进程
# --quiet 只打印错误信息

if [ $# -lt 1 ]; then
	echo "usage: $0 [program]"
	exit
fi

# --valgrind-stacksize=<number> [default: 1MB]
# valgrind: '--valgrind-stacksize' argument must be between 8192 and 10485760

valgrind --leak-check=full \
		 --show-reachable=yes \
		 --trace-children=yes \
		 --valgrind-stacksize=8192 \
		 --quiet \
		 $*
