#!/bin/bash
# 配置内核实时性
# 荣涛 2021年2月19日

# root
if [ $UID != 0 ]; then
	echo "must be root."
	exit
fi

for script in `ls scripts` 
do
	. scripts/$script
done


echo -e "\n ------------------------- RealTime Configure sysctl --------------------------"
rt_config_sysctl

echo -e "\n ------------------------- RealTime Configure BIOS ----------------------------"
rt_config_BIOS

echo -e "\n ------------------------- RealTime Configure IRQs ----------------------------"
rt_config_IRQs

echo -e "\n ------------------------- RealTime Configure DONE ----------------------------"

