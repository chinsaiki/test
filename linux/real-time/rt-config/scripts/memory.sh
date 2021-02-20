#!/bin/bash
# 荣涛 2021年2月20日
# 内存配置

file_panic_on_oom="/proc/sys/vm/panic_on_oom"

function swapon8_help {
	echo -e "[MEM]\n[MEM] \033[1m swapon(8) 帮助信息\033[m"	
	echo -e "[MEM]  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"

	swapon --help | sed -n '2,30p' | sed 's/^/[MEM]  >>&/g'
}
function swapon2_help {
	echo -e "[MEM]\n[MEM] \033[1m swapon(2) 帮助信息\033[m"	
	echo -e "[MEM]  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"

	man 2 swapon | sed -n '5,15p' | sed 's/^/[MEM]  >>&/g'
}

function swapoff_help {
	echo -e "[MEM]\n[MEM] \033[1m swapoff 帮助信息\033[m"	
	echo -e "[MEM]  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"

	swapoff --help | sed -n '2,30p' | sed 's/^/[MEM]  >>&/g'
}

function vmstat_help {
	echo -e "[MEM]\n[MEM] \033[1m vmstat 帮助信息\033[m"	
	echo -e "[MEM]  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"

	vmstat --help | sed 's/^/[MEM]  >>&/g'
}
function __rt_config_memory_command_manual {
	echo -e "[MEM]\n[MEM] =============== 显示 内存/交换空间相关 手册 信息"
	
	memory_manual_options="[swapon2|swapon8|swapoff|vmstat|回车退出]"
	while read -r -p "[MEM] 选择手册 $memory_manual_options: " man_type
	do
		case $man_type in
		swapon2)
			swapon2_help
		;;
		swapon8)
			swapon8_help
		;;
		swapoff)
			swapoff_help
		;;
		vmstat)
			vmstat_help
		;;
		exit|quit)
			exit 1
		;;
		*) 
			if [ -z $man_type ]; then
				break
			fi
			
		;;
		esac
	done
}

#内存不足（OOM）
function __rt_config_memory_OOM {
	
	echo -e "[MEM]\n[MEM] =============== 配置内存/交换空间"
	echo -e "[MEM]"
	echo -e "[MEM]   内存不足（OOM）表示已分配所有可用内存（包括交换空间）的计算状态。通常，这将导致"
	echo -e "[MEM] 系统出现紧急情况并按预期停止运行。中有一个用于控制OOM行为的开关/proc/sys/vm/pani-"
	echo -e "[MEM] c_on_oom。设置为1时，内核将对OOM感到恐慌。默认设置是0指示内核调用oom_killerOOM上命"
	echo -e "[MEM] 名的函数的默认设置。通常，它oom_killer可以杀死恶意进程，并且系统可以生存。"
	echo -e "[MEM]"
	echo -e "[MEM] 建议您进行$ RT；内核对OOM感到恐慌。当系统遇到OOM状态时，它将不再是确定性的。"
	echo -e "[MEM]"
	echo 1 > $file_panic_on_oom 2>&1 > /dev/nul
}

function rt_config_memory {
	__rt_config_memory_OOM
	__rt_config_memory_command_manual
}

#rt_config_memory
