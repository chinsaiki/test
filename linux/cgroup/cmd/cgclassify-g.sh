#!/bin/bash
# 绑定进程到 cgroup 上

# 首先约定好 这个 cgroup 的CPU利用率最高只能 30%（所有进程将平分）
echo 30000 >  /sys/fs/cgroup/cpu/rongtao/cpu.cfs_quota_us

# 使用 while 启动 占用 100% 的进程
while :; do :; done &
while :; do :; done &

# 根据实际 PID 情况而定
cgclassify -g cpu:rongtao 181481
cgclassify -g cpu:rongtao 181482

# 查看 当前 的 /sys/fs/cgroup/cpu/rongtao/tasks
more /sys/fs/cgroup/cpu/rongtao/tasks 
# 181453
# 181481
