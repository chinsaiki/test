#!/bin/bash
# 配置内核实时性
# 荣涛 2021年2月19日 创建


# 主函数
function __main__ {

echo -e " #######################################################################################"
echo -e " ##   "
echo -e " ##                              CentOS 服务器实时性配置	"
echo -e " ##   "
echo -e " ##	为服务器的 \"实时配置\" 提供支持，通过配置 sysctl(系统参数)，BIOS，中断，调度， "
echo -e " ## 文件系统等内容，满足服务器的 \"实时性配置\".	"
echo -e " ##	"
echo -e " ##                                                                  作者：荣涛"
echo -e " ##                                                                  日期：2021年2月"
echo -e " #######################################################################################"
echo -e " ------------------------- RealTime Configure BEGIN ----------------------------"

read -r -p "[RT-config] 按任意键继续: " __tmp

echo -e "\n[RT-config] 你想要 配置 或者 显示 哪些 信息？"
echo -e "[RT-config] 		all		所有"
echo -e "[RT-config] 		sysctl		sysctl 相关配置"
echo -e "[RT-config] 		bios		BIOS 相关配置"
echo -e "[RT-config] 		irqs		IRQs 中断 相关配置"
echo -e "[RT-config] 		sched		schedule 调度 相关配置"
echo -e "[RT-config] 		fs		fs 文件系统 相关配置"
echo -e "[RT-config] 		clock		clock 时钟 相关配置"
echo -e "[RT-config] 		memory		内存/交换空间 相关配置"
echo -e "[RT-config] 		tcp		TCP 相关配置"
echo -e "[RT-config] 		syslog		SYSlog 相关配置"
echo -e "[RT-config] 		tuned		tuned 相关配置"


echo -e "[RT-config]"

main_options="[all|sysctl|bios|irqs|sched|fs|clock|memory|tcp|syslog|tuned]"

while read -r -p "[RT-config] 选择 $main_options: " infomation
do
	
	case $infomation in
	all)
		echo -e "\n ------------------------- RealTime Configure ALL --------------------------"
		rt_config_sysctl
		rt_config_BIOS
		rt_config_IRQs
		rt_config_sched
		rt_config_fs
		rt_config_clock
		rt_config_memory
		rt_config_tcp
		rt_config_syslog
		rt_config_tuned
	;;
	sysctl)
		echo -e "\n ------------------------- RealTime Configure sysctl --------------------------"
		rt_config_sysctl
	;;
	bios)
		echo -e "\n ------------------------- RealTime Configure BIOS ----------------------------"
		rt_config_BIOS
	;;
	irqs)
		echo -e "\n ------------------------- RealTime Configure IRQs ----------------------------"
		rt_config_IRQs
	;;
	sched)
		echo -e "\n ------------------------- RealTime Configure SCHED ---------------------------"
		rt_config_sched
	;;
	fs)
		echo -e "\n ------------------------- RealTime Configure FS ------------------------------"
		rt_config_fs
	;;
	clock)
		echo -e "\n ------------------------- RealTime Configure CLOCK ---------------------------"
		rt_config_clock
	;;
	memory)
		echo -e "\n ------------------------- RealTime Configure MEMORY --------------------------"
		rt_config_memory
	;;
	tcp)
		echo -e "\n ------------------------- RealTime Configure TCP -----------------------------"
		rt_config_tcp
	;;
	syslog)
		echo -e "\n ------------------------- RealTime Configure SYSlog --------------------------"
		rt_config_syslog
	;;
	tuned)
		echo -e "\n ------------------------- RealTime Configure TUNED ---------------------------"
		rt_config_tuned
	;;
	exit|quit)
		echo "[RT-config] Exit."
		exit 1
	;;
	*) 
		if [ ! -z $infomation ]; then
			echo -e '[RT-config]		\033[1;31m错误的输入！\033[m'需要$main_options
		fi
	;;
	esac
	echo -e "[RT-config]"
done

echo -e "\n ------------------------- RealTime Configure DONE ----------------------------"
}


####################################################################################################
# 开始执行
####################################################################################################
# root
if [ $UID != 0 ]; then
	echo "must be root."
	exit
fi

for script in `ls scripts/*.sh` 
do
	. $script
done

__main__ 
