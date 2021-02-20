#!/bin/bash
# 荣涛 2021年2月20日
# 时钟配置

file_available_clocksource="/sys/devices/system/clocksource/clocksource0/available_clocksource"
file_current_clocksource="/sys/devices/system/clocksource/clocksource0/current_clocksource"


function __rt_config_clock_source {

	echo -e "[CLK]\n[CLK] =============== 配置系统时钟"
	echo -e "[CLK]\n[CLK] 诸如NUMA或SMP之类的多处理器系统具有多个硬件时钟实例。"
	echo -e "[CLK]"
	echo -e "[CLK]   在启动期间，内核会发现可用的时钟源并选择要使用的时钟源。有时由于时钟上"
	echo -e "[CLK] 的已知问题，未使用系统主应用程序中性能最佳的时钟。排除所有有问题的时钟后，"
	echo -e "[CLK] 系统将剩下无法满足实时系统最低要求的硬件时钟。"
	echo -e "[CLK]"
	echo -e "[CLK]   关键应用程序的要求在每个系统上都不同。因此，每种应用的最佳时钟，进而每"
	echo -e "[CLK] 个系统的最佳时钟也各不相同。某些应用程序取决于时钟分辨率，并且提供可靠的"
	echo -e "[CLK] 纳秒读数的时钟可能更合适。经常读取时钟的应用程序可以受益于时钟成本较低的"
	echo -e "[CLK] 时钟（读取请求和结果之间的时间）。"
	echo -e "[CLK]"
	echo -e "[CLK]   尽管没有单个时钟适合所有系统，但是TSC通常是首选的时钟源。为了优化TSC时"
	echo -e "[CLK] 钟的可靠性，可以在引导内核时配置其他参数，例如："
	echo -e "[CLK]"
	echo -e "[CLK]   \"idle=poll\"：强制时钟避免进入空闲状态。"
	echo -e "[CLK]   \"processor.max_cstate=1\"：防止时钟进入更深的C状态（节能模式），因此"
	echo -e "[CLK]                               它不会变得不同步。"
	echo -e "[CLK]"
	echo -e "[CLK] 但是请注意，在这两种情况下，能耗都会增加，因为系统将始终以最高速度运行。"
	echo -e "[CLK]"
	
	read -r -p "[CLK] 按任意键继续: " __tmp

	available_clocksource=`cat $file_available_clocksource`
	current_clocksource=`cat $file_current_clocksource`
	
	echo $available_clocksource | sed "s/^/[CLK]   可用的时钟源： &/g"
	echo $current_clocksource | sed "s/^/[CLK]   当前的时钟源： &/g"
	echo -e "[CLK]"

	all_available_clocksource_selector=`echo $available_clocksource | sed "s/ /|/g" | awk '{print "["$1"]"}'`
	#echo $all_available_clocksource_selector
	#kvm-clock tsc hpet acpi_pm
	
	while read -r -p "[CLK] 选择下面的时钟源 $all_available_clocksource_selector: " selected_clk_src
	do
		case $selected_clk_src in
		exit|quit)
			exit 1
		;;
		*) 
			if [ ! -z $selected_clk_src ]; then
				echo $available_clocksource | sed 's/-/XX/g' | grep -w `echo $selected_clk_src | sed 's/-/XX/g'` 2>&1 > /dev/null
				if [ $? == 0 ]; then
					if [ $selected_clk_src == $current_clocksource ]; then
						echo "[CLK]	当前时钟源已经是 $selected_clk_src"
					else
						echo "[CLK]	配置时钟源 从 \"$current_clocksource\" 到 \"$selected_clk_src\""
						echo $selected_clk_src > $file_current_clocksource
					fi
					break
				fi
			fi
			echo -e '[CLK]		\033[1;31m,,错误的输入！\033[m'需要 $all_available_clocksource_selector
		;;
		esac
	done
}

function rt_config_clock {
	__rt_config_clock_source
}

#rt_config_clock
