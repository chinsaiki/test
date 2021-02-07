#!/bin/bash

USERNAME=root
IPADDR=10.170.6.66
DST_PATH=/work/workspace/test/linux/LinuxKernelLearnCode

if [ $# -lt 1 ]; then
	echo $0 [file or directory]
	exit
fi

if [ -d $1 ]; then
	scp -r $1 $USERNAME@$IPADDR:$DST_PATH
elif [ -f $1 ]; then
	scp $1 $USERNAME@$IPADDR:$DST_PATH
else
	echo Error file type.
fi

