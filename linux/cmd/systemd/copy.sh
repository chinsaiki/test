#!/bin/bash
# 拷贝源文件到我的 test 仓库中
# rtoax 荣涛 2021年1月5日

if [ $# -lt 1 ]; then
	echo "usage: $0 [path/to/file]"
	exit
fi
file=$1
dst_dir="./"

if [ -f $file ]; then
	swapfile=$(echo $file | sed "s/\//-/g")
	 #  不要第一个 "-"
	echo Copy ${swapfile:1} to $dst_dir 
	cp $file $dst_dir/${swapfile:1} 
	chmod 0666 $dst_dir/${swapfile:1}
else
	echo "Wrong file TYPE."	
fi

