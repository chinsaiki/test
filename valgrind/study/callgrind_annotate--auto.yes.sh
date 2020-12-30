#!/bin/bash

# --tool=callgrind 检查代码覆盖和性能瓶颈

if [ $# -lt 1 ]; then
	echo "usage: $0 [program]"
	exit
fi

#valgrind --tool=callgrind \
#	$*

# 然后调用
callgrind_annotate --auto=yes \
	$*

