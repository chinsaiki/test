#!/bin/bash
# 荣涛 2021年2月19日

PROC_IRQs="/proc/interrupts"

g_cpu_list=""

# 处理每个系统中的中断
# $1 == handler
# $2 == handler's arguments
function __for_each_IRQ() {
	if [ $# -lt 1 ]; then
		echo "$0 [handler]"
		exit 1
	fi
	
	handler=$1
	handler_args=""
	if [ $# == 2 ]; then
		handler_args=`$2`
	fi
	
	# 清空全局变量
	g_cpu_list=""
	
	IRQs=`cat $PROC_IRQs | grep -E "[0-9A-Z]\\w*:" | awk '{print $1}'`
	for irq in $IRQs
	do
		# 多虑一些中断，配置不成功的
		if [ $irq != "30" ] || [ $irq != 37 ]; then
			#去除 最后 一个 ":"
			$handler ${irq%?} $handler_args
		fi
	done
	return 0
}

# 是否是 系统的 IRQ 序号
function __is_IRQ_number() {
	if [ $# -lt 1 ]; then
		return 1
	fi
	# 此 IRQ存在
	if [ -d /proc/irq/$1 ]; then
		return 0
	else
		return 2
	fi
}

function __rt_config_set_irq_smp_affinity_list() {
#/proc/interrupts
#            CPU0       CPU1       CPU2       CPU3  中断控制器        irq		驱动程序
#   1:          0          9          0          0   IO-APIC    1-edge      i8042
#   6:          0          0          0          3   IO-APIC    6-edge      floppy

#smp_affinity            irq和cpu之间的亲缘绑定关系；
#smp_affinity_hint   只读条目，用于用户空间做irq平衡只用；
#spurious                  可以获得该irq被处理和未被处理的次数的统计信息；
#handler_name       驱动程序注册该irq时传入的处理程序的名字；

	__is_IRQ_number $1
	if [ $? != 0 ]; then
		#echo "__is_IRQ_number [IRQ number]"
		return 1
	fi
	
	cpu_list=""
	irq_item=`cat /proc/interrupts | grep " $1:" | awk '{print $(NF-3)" "$(NF-2)" "$(NF-1)" " $(NF)}'`
	IRQ_ITEM=""
	all_IRQ_cpu_list=""


	if [ $# == 2 ]; then
		all_IRQ_cpu_list=$2
	fi
	
	for i in $irq_item; 
	do 
		echo "$i"|[ -n "`sed -n '/^[0-9][0-9]*$/p'`" ] ; 
		if [ $? != 0 ]; then
			IRQ_ITEM=$IRQ_ITEM"$i "
		fi
	done
	
	Introduction=`echo $IRQ_ITEM | awk '{print " 中断控制器:"$1", 中断名称："$2", 驱动:"$3$4}'`

	curr_cpu_list=`cat /proc/irq/$1/smp_affinity_list`


	if [ -z $all_IRQ_cpu_list ]; then	
	
		echo -e "[IRQ] 中断号:\033[1;31m$1\033[m $Introduction"
		
		read -r -p "[IRQ]      该中断的 CPUs 将从 \"$curr_cpu_list\" 设置为[例:0,2-3]: " cpu_list

		# 如果输入的list不合法
		while echo $cpu_list | grep -E "^([0-9]\\w*[,|-])*[0-9]\\w*$" 2>&1 > /dev/null
		[ $? != 0 ]
		do
			read -r -p "[IRQ]      该中断的 CPUs 将从 \"$curr_cpu_list\" 设置为[例:0,2-3]: " cpu_list
		done
	else
		cpu_list=$all_IRQ_cpu_list
	fi
	
	# 设置对应的 CPU LIST
	if [ -z $cpu_list ]; then
		if [ -z $g_cpu_list ]; then
			echo -e "\n请输入详细的 CPU list。\n"
			exit 1
		fi
		echo -e "[IRQ]      使用上一次的 CPU list \"$g_cpu_list\" 配置 \033[1;31m$1\033[m."
		echo $g_cpu_list > /proc/irq/$1/smp_affinity_list
		
	# 更新全局的 CPU list，直接回车的情况
	else
		g_cpu_list=$cpu_list
		echo $cpu_list > /proc/irq/$1/smp_affinity_list 2>/dev/null
		echo -e "[IRQ]      中断号:\033[1;31m$1\033[m 从 \"$curr_cpu_list\" 设置为 \"$cpu_list\" 成功。"
	fi
	
	#echo "echo $cpu_list > /proc/irq/$1/smp_affinity_list"
	
	return 0
}

function __rt_config_set_all_irq_smp_affinity_list_args() {
	
	read -r -p "[IRQ]所有的中断 的 CPU list 将 设置为[例:0,2-3]: " cpu_list 
	# 如果输入的list不合法
	while echo $cpu_list | grep -E "^([0-9]\\w*[,|-])*[0-9]\\w*$" 2>&1 > /dev/null
	[ $? != 0 ]
	do
		read -r -p "[IRQ]不合法的输入 $cpu_list [例:0,2-3]: " cpu_list 
	done
	echo $cpu_list
}

function __rt_config_set_all_irq_smp_affinity_list() {
	
	__for_each_IRQ __rt_config_set_irq_smp_affinity_list  __rt_config_set_all_irq_smp_affinity_list_args
}


#禁用irqbalance守护程序
function __rt_config_disable_irqblance() {
	systemctl status irqbalance	2>&1 > /dev/null
	systemctl stop irqbalance	2>&1 > /dev/null
	systemctl disable irqbalance	2>&1 > /dev/null
	echo "[IRQ]config \"systemctl disable irqbalance\"."
}

#从IRQ Balancing中排除CPU
function __rt_config_disable_cpu_irqblance() {
	echo "[IRQ]config \"IRQBALANCE_BANNED_CPUS=\" in \"/etc/sysconfig/irqbalance\". \033[1;31mTODO...\033[m"
}

#__for_each_IRQ __rt_config_set_irq_smp_affinity_list 
#__rt_config_set_all_irq_smp_affinity_list

#__rt_config_disable_irqblance
#__rt_config_disable_cpu_irqblance

function rt_config_IRQs() {
	__rt_config_disable_irqblance
	__rt_config_disable_cpu_irqblance
	__rt_config_set_all_irq_smp_affinity_list
}

