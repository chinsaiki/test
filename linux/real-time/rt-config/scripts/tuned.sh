#!/bin/bash
# 荣涛 2021年2月20日
# 配置 tuned

file_realtime_variables_conf="/etc/tuned/realtime-variables.conf"
file_tuned_conf="/usr/lib/tuned/realtime/tuned.conf"

function __rt_config_tuned_yum {

	echo -e "[TUNED]\n[TUNED] =============== 检查 tuned-profiles-realtime 是否安装"

	echo -e "[TUNED]"
	
	yum list tuned-profiles-realtime | grep tuned-profiles-realtime 2>&1 > /dev/null
	if [ $? != 0 ]; then
		echo -e "[TUNED] tuned-profiles-realtime 未安装." 
		echo -e "[TUNED]  ~]# yum install -y tuned-profiles-realtime" 
		return 1
	fi
	return 0
}

function __rt_config_tuned_config {
	echo -e "[TUNED]\n[TUNED] =============== TUNED 服务器调优"
	echo -e "[TUNED]config \033[1;31mTODO...\033[m, 参见："

}

function rt_config_tuned {
	__rt_config_tuned_yum
	if [ $? != 0 ]; then
		return 1
	fi
	__rt_config_tuned_config
	return 0
}
