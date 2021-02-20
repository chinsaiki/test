#!/bin/bash
# 日期 2021年2月19日
# 作者 荣涛


# 电源管理
function __rt_config_bios_pm() {
	echo -e "[BIOS]\n[BIOS] =============== 禁用电源管理"

	while true
	do
		read -r -p "[BIOS]为了获得最佳响应时间，已经确定在BIOS中禁用电源管理选项? [Y/n] " input
		case $input in
			[yY][eE][sS]|[yY])
				#echo "Yes"
				return 0
				;;

			[nN][oO]|[nN])
				echo -e "[BIOS]\n[BIOS] \033[1;31m请重启服务器，配置 BIOS 电源管理选项。\033[m"
				echo -e "[BIOS]"
				exit 1
			   	;;

			*)
				#echo "Invalid input..."
				#exit 1
				;;
		esac
	done
}

#EDAC单元是用于检测和纠正从纠错码（ECC）存储器发出的错误的设备。
#通常，EDAC选项的范围从无ECC检查到定期扫描所有内存节点是否有错误。
#EDAC级别越高，BIOS花费的时间就越多，并且错过关键事件的截止日期的可能性也就越大。
#如果可能，请关闭EDAC。否则，请切换到最低功能级别。
function __rt_config_bios_EDAC() {
	echo -e "[BIOS]\n[BIOS] =============== 关闭EDAC(检测和纠正从纠错码)"

	while true
	do
		read -r -p "[BIOS]如果可能，请关闭EDAC(检测和纠正从纠错码)。否则，请切换到最低功能级别? [Y/n] " input
		case $input in
			[yY][eE][sS]|[yY])
				#echo "Yes"
				return 0
				;;

			[nN][oO]|[nN])
				echo -e "[BIOS]\n[BIOS] \033[1;31m请重启服务器，配置 BIOS 关闭EDAC(检测和纠正从纠错码)。\033[m"
				echo -e "[BIOS]"
				exit 1
			   	;;

			*)
				#echo "Invalid input..."
				#exit 1
				;;
		esac
	done
}

#系统管理中断（SMI）
#SMI是硬件供应商使用的工具，可确保系统正常运行。
#SMI中断通常不由运行的操作系统服务，而是由BIOS中的代码服务。
#SMI通常用于热量管理，远程控制台管理（IPMI），EDAC检查以及其他各种内部管理任务。
#如果BIOS包含SMI选项，请与供应商和任何相关文档联系以检查禁用它们的安全程度。
#
#警告:尽管可以完全禁用SMI，但强烈建议您不要这样做。删除系统生成和服务SMI的功能可能会导致灾难性的硬件故障。
function __rt_config_bios_SMI() {
	echo -e "[BIOS]\n[BIOS] =============== 禁用SMI(系统管理中断)"

	while true
	do
		read -r -p "[BIOS]如果BIOS包含SMI(系统管理中断)选项，请与供应商联系以检查禁用它的安全程度? [Y/n] " input
		case $input in
			[yY][eE][sS]|[yY])
				#echo "Yes"
				return 0
				;;

			[nN][oO]|[nN])
				echo -e "[BIOS]\n[BIOS] \033[1;33m你可能需要重启服务器，配置 BIOS 禁用SMI(系统管理中断)。\033[m"
				echo -e "[BIOS]"
				return 1
			   	;;

			*)
				#echo "Invalid input..."
				#exit 1
				;;
		esac
	done
}

function rt_config_BIOS() {
	__rt_config_bios_pm
	__rt_config_bios_EDAC
	__rt_config_bios_SMI
}

