#!/bin/bash

# 关闭时间戳：
sudo sysctl -w net.ipv4.tcp_timestamps=0

# 打开时间戳
sudo sysctl -w net.ipv4.tcp_timestamps=1

#显示
sudo sysctl net.ipv4.tcp_timestamps
