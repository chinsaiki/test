#!/bin/bash

# --leak-check=full 完全检查内存泄漏
# --show-reachable=yes 显示内存泄漏地点
# --trace-children=yes 跟入子进程
# --quiet 只打印错误信息

if [ $# -lt 1 ]; then
	echo "usage: $0 [program]"
	exit
fi

# --xtree-memory=none|allocs|full [none]

xtfile="xtmemory.kcg.out"

valgrind --leak-check=full \
		 --show-reachable=yes \
		 --trace-children=yes \
		 --xtree-memory=full \
		 --xtree-memory-file=$xtfile \
		 --quiet \
		 $*

# 使用 如下命令查看
callgrind_annotate --auto=yes \
				   --inclusive=yes \
				   --sort=curB:100,curBk:100,totB:100,totBk:100,totFdB:100,totFdBk:100  \
				   $xtfile

rm -f $xtfile
