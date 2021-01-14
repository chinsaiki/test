#!/bin/bash

#系统时钟，每秒钟产生的时钟硬件中断，tick就是连续两个中断的间隔时间
cat /boot/`uname -r` | grep "CONFIG_HZ="
cat /boot/config-3.10.0-693.el7.x86_64 | grep "CONFIG_HZ="
