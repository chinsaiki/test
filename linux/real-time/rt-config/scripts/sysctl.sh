#!/bin/bash

rc_local="/etc/rc.local"

function __rt_config_sysctl_addto_rc_local() {
	grep "^$*$" $rc_local 2>&1 > /dev/null
	if [ $? != 0 ]; then
		echo $* >> $rc_local
	else
		echo "[SYSCTL]config \"$*\" add to \"$rc_local\" already."
	fi
}

function rt_config_sysctl() {
	echo "[SYSCTL]config kernel.hung_task_panic = 0"
	sudo sysctl -w kernel.hung_task_panic=0 2>&1 > /dev/null
	__rt_config_sysctl_addto_rc_local sysctl -w kernel.hung_task_panic=0
}

