#!/bin/bash
# 荣涛2021年2月20日 
# TCP 网络相关【配置



function __rt_config_tcp_sysctl_addto_rc_local() {
	grep "^$*$" $rc_local 2>&1 > /dev/null
	if [ $? != 0 ]; then
		echo $* >> $rc_local
	else
		echo "[TCP]config \"$*\" add to \"$rc_local\" already."
	fi
}

function __rt_config_tcp_sysctl_tcp_timestamps() {
	echo -e "[TCP]\n[TCP] =============== 配置net.ipv4.tcp_timestamps"

	echo -e "[TCP]	1. 使用以下命令关闭时间戳："
	echo -e "[TCP]		~]# sysctl -w net.ipv4.tcp_timestamps=0"
	echo -e "[TCP]		net.ipv4.tcp_timestamps = 0"
	echo -e "[TCP]"

	echo -e "[TCP]	2. 使用以下命令打开时间戳："
	echo -e "[TCP]		~]# sysctl -w net.ipv4.tcp_timestamps=1"
	echo -e "[TCP]		net.ipv4.tcp_timestamps = 1"
	echo -e "[TCP]"

	echo -e "[TCP]	3. 使用以下命令打印当前值："
	echo -e "[TCP]		~]# sysctl net.ipv4.tcp_timestamps"
	echo -e "[TCP]		net.ipv4.tcp_timestamps = 1"
	echo -e "[TCP]"
	
	echo "[TCP]config net.ipv4.tcp_timestamps = 0"
	sudo sysctl -w net.ipv4.tcp_timestamps=0 2>&1 > /dev/null
	__rt_config_tcp_sysctl_addto_rc_local sysctl -w net.ipv4.tcp_timestamps=0
}


function __rt_config_tcp_MORE {
	echo -e "[TCP]\n[TCP] =============== TCP 网络调优"
	echo -e "[TCP]config \033[1;31mTODO...\033[m, 参见："
	echo -e "[TCP]减少TCP性能峰值: https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux_for_real_time/7/html/tuning_guide/Reduce_TCP_performance_spikes"
	echo -e "[TCP]减少TCP延迟的ACK超时: https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux_for_real_time/7/html/tuning_guide/Reducing_the_TCP_delayed_ack_timeout"

}

function rt_config_tcp {
	__rt_config_tcp_MORE
	__rt_config_tcp_sysctl_tcp_timestamps
	
}
