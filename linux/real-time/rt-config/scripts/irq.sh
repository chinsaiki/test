#!/bin/bash

#禁用irqbalance守护程序
function __rt_config_disable_irqblance() {
	systemctl status irqbalance	2>&1 > /dev/null
	systemctl stop irqbalance	2>&1 > /dev/null
	systemctl disable irqbalance	2>&1 > /dev/null
	echo "[IRQs]config \"systemctl disable irqbalance\"."
}

#从IRQ Balancing中排除CPU
function __rt_config_disable_cpu_irqblance() {
	echo "[IRQs]config \"IRQBALANCE_BANNED_CPUS=\" in \"/etc/sysconfig/irqbalance\"."
	#TODO
}

#__rt_config_disable_irqblance
#__rt_config_disable_cpu_irqblance