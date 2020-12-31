#!/bin/bash

# --tool=memcheck
# --quiet 只打印错误信息
# 1、memcheck： 分析内存
# 2、callgrind：    分析程序性能。
# 3、cachegrind：	分析cache.
# 4、helgrind：     分析多线程竞争。
# 5、massif：       分析堆。

if [ $# -lt 1 ]; then
	echo "usage: $0 [memcheck|callgrind|cachegrind|helgrind|massif|...]"
	exit
fi

valgrind --tool=$1 \
		 --help-dyn-options
