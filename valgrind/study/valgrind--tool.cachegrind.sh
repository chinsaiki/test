#!/bin/bash

# --tool=cachegrind 
# --quiet 只打印错误信息

if [ $# -lt 1 ]; then
	echo "usage: $0 [program]"
	exit
fi

valgrind --tool=cachegrind \
		 --quiet \
		$*

# 然后调用
#callgrind_annotate --auto=yes \
#				callgrind.out.[pid]

