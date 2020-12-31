#!/bin/bash

# --leak-check=full 完全检查内存泄漏
# --show-reachable=yes 显示内存泄漏地点
# --trace-children=yes 跟入子进程
# --quiet 只打印错误信息

# --trace-children=<yes|no> [default: no]
# --trace-children-skip=patt1,patt2,...
# --trace-children-skip-by-arg=patt1,patt2,...
# --child-silent-after-fork=<yes|no> [default: no]
# 

if [ $# -lt 1 ]; then
	echo "usage: $0 [program]"
	exit
fi

valgrind --leak-check=full \
		 --show-reachable=yes \
		 --trace-children=yes \
		 --error-limit=no \
		 --trace-children=yes \
		 --quiet \
		 $*
