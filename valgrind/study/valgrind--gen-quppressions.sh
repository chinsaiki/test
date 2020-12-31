#!/bin/bash

# --tool=memcheck
# --quiet 只打印错误信息

if [ $# -lt 1 ]; then
	echo "usage: $0 [program]"
	exit
fi

#  --default-suppressions=no
#  --suppressions=/path/to/file.supp
valgrind --tool=memcheck \
		 --gen-suppressions=yes \
		 --quiet \
		$*

# 然后调用
#callgrind_annotate --auto=yes \
#				callgrind.out.[pid]


