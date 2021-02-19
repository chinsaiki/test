#!/bin/bash
# 配置内核实时性
# 荣涛 2021年2月19日

if [ $UID != 0 ]; then
	echo "must be root."
	exit
fi

for script in `ls scripts` 
do
	. scripts/$script
done



rt_config_sysctl
