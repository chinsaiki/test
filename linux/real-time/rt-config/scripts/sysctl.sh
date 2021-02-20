#!/bin/bash
# 荣涛 2021年2月19日

rc_local="/etc/rc.local"

function __rt_config_sysctl_addto_rc_local() {
	grep "^$*$" $rc_local 2>&1 > /dev/null
	if [ $? != 0 ]; then
		echo $* >> $rc_local
	else
		echo "[SYSCTL]config \"$*\" add to \"$rc_local\" already."
	fi
}


function __rt_config_sysctl_kernel_hung_task_panic() {
	echo -e "[SYSCTL]\n[SYSCTL] =============== 配置kernel.hung_task_panic"

	echo "[SYSCTL]config kernel.hung_task_panic = 0"
	sudo sysctl -w kernel.hung_task_panic=0 2>&1 > /dev/null
	__rt_config_sysctl_addto_rc_local sysctl -w kernel.hung_task_panic=0
}

function __rt_config_systemctl_set_default_multi_user {
	echo -e "[SYSCTL]\n[SYSCTL] =============== 关闭图形界面"

	systemctl get-default | sed 's/^/[SYSCTL]  当前运行级别为：&/g'
	systemctl set-default multi-user.target 2>&1 > /dev/null
	echo "[SYSCTL]  将运行级别设置为：multi-user.target"
	
}
function __rt_config_systemctl_disable_pcscd {
	echo -e "[SYSCTL]\n[SYSCTL] =============== 关闭PC卡守护程序"


	echo -e "[SYSCTL]\n[SYSCTL] pcscd状态"
	echo -e "[SYSCTL]  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
	systemctl status pcscd | sed -n '1,3p' | sed 's/^/[SYSCTL]  >>&/g'
	echo -e "[SYSCTL]\n[SYSCTL] pcscd.socket状态"
	echo -e "[SYSCTL]  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
	systemctl status pcscd.socket | sed -n '1,3p' | sed 's/^/[SYSCTL]  >>&/g'

	echo -e "[SYSCTL]\n[SYSCTL] 关闭 pcscd 守护进程"
	systemctl stop pcscd.socket 2>&1 > /dev/null
	systemctl stop pcscd 2>&1 > /dev/null

	echo -e "[SYSCTL] Disable pcscd 守护进程"
	systemctl disable pcscd.socket 2>&1 > /dev/null
	systemctl disable pcscd 2>&1 > /dev/null
}

function __rt_config_sysctl_show {

	echo -e "[SYSCTL]\n[SYSCTL] sysctl 参数状态"

	echo -e "[SYSCTL]  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
	
	sysctl kernel.sched_rt_runtime_us | sed 's/^/[SYSCTL]  >>&/g'
	sysctl kernel.hung_task_timeout_secs | sed 's/^/[SYSCTL]  >>&/g'

	#sysctl -a | sed 's/^/[SYSCTL]  >>&/g'
}

function rt_config_sysctl() {
	__rt_config_sysctl_kernel_hung_task_panic
	__rt_config_systemctl_set_default_multi_user
	__rt_config_systemctl_disable_pcscd

	__rt_config_sysctl_show
	
}
