#!/bin/bash
# 

function taskset_help {	
	echo -e "[SCHED]\n[SCHED] \033[1m taskset 帮助信息\033[m"	
	echo -e "[SCHED]  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"

	taskset --help | sed 's/^/[SCHED]  >> &/g'
}

function chrt_help {
	echo -e "[SCHED]\n[SCHED] \033[1m chrt 帮助信息\033[m"	
	echo -e "[SCHED]  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"

	chrt --help | sed 's/^/[SCHED]  >> &/g'
}
function nice_help {
	echo -e "[SCHED]\n[SCHED] \033[1m nice 帮助信息\033[m"	
	echo -e "[SCHED]  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"

	nice --help | head -10 | sed 's/^/[SCHED]  >> &/g'
}

function renice_help {
	echo -e "[SCHED]\n[SCHED] \033[1m renice 帮助信息\033[m"	
	echo -e "[SCHED]  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"

	renice --help | sed 's/^/[SCHED]  >> &/g'
}
function sched_setscheduler_help {
	echo -e "[SCHED]\n[SCHED] \033[1m sched_setscheduler 帮助信息\033[m"	
	echo -e "[SCHED]  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"

	man sched_setscheduler | sed -n "5,20p" | sed 's/^/[SCHED]  >> &/g'
}

function __rt_config_sched_manual {
	echo -e "[SCHED]\n[SCHED] =============== 显示 调度配置相关 手册 信息"
	while read -r -p "[SCHED] 选择手册 [taskset|nice|renice|chrt|sched_setscheduler|回车退出]: " man_type
	do
		
		case $man_type in
		taskset)
			taskset_help
		;;
		nice)
			nice_help
		;;
		renice)
			renice_help
		;;
		chrt)
			chrt_help
		;;
		sched_setscheduler)
			sched_setscheduler_help
		;;
		*) 
			break
		;;
		esac
	done
}

function __rt_config_sched_taskset_usage {
	echo -e "[SCHED]\n[SCHED] =============== 使用 taskset 设置 CPU 亲和性"	
	echo -e "[SCHED]  例1: taskset 8 exe (将 exe 运行在 CPU 3（使用CPU掩码的十进制版本） 上)"
	echo -e "[SCHED]  例2: taskset -p 1 7013 (指示PID为7013的进程仅在CPU 0上运行)"
	echo -e "[SCHED]  例3: taskset 0xF0 exe (exe 被指示在处理器4、5、6和7上执行)"
	echo -e "[SCHED]  例4: 使用-c参数，您可以指定CPU列表而不是CPU掩码"

	echo -e "[SCHED]"
	while true
	do
		read -r -p "[SCHED]	我已明确 taskset 的使用? [Y/n] " input
		case $input in
			[yY][eE][sS]|[yY])
				#echo "Yes"
				return 0
				;;

			[nN][oO]|[nN]|*)
				#echo "No"
				taskset_help
				echo -e "[SCHED]"
				#exit 1
			   	;;
		esac
	done
}
function rt_config_sched() {
	__rt_config_sched_taskset_usage

	__rt_config_sched_manual
}

#rt_config_sched
