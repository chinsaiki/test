#!/bin/bash
# 拷贝源文件到我的 test 仓库中
# rtoax 荣涛

if [ $# -lt 1 ]; then
	echo "usage: $0 [path/to/file]"
	exit
fi
file=$1
dst_dir="../test/linux-5.0/"

if [ -f $file ]; then
	swapfile=$(echo $file | sed "s/\//-/g")
	echo Copy $swapfile to $dst_dir
	cp $file $dst_dir/$swapfile
else
	echo "Wrong file TYPE."	
fi

