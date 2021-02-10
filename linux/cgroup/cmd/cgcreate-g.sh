#!/bin/bash
# 创建CPU cgroup
cgcreate -g cpu:rongtao

#ll /sys/fs/cgroup/cpu/
#总用量 0
#...
#drwxr-xr-x   2 root root 0 2月  10 08:47 rongtao

# tree /sys/fs/cgroup/cpu/rongtao/
#/sys/fs/cgroup/cpu/rongtao/
#├── cgroup.clone_children
#├── cgroup.procs
#├── cpuacct.stat
#├── cpuacct.usage
#├── cpuacct.usage_all
#├── cpuacct.usage_percpu
#├── cpuacct.usage_percpu_sys
#├── cpuacct.usage_percpu_user
#├── cpuacct.usage_sys
#├── cpuacct.usage_user
#├── cpu.cfs_period_us
#├── cpu.cfs_quota_us
#├── cpu.rt_period_us
#├── cpu.rt_runtime_us
#├── cpu.shares
#├── cpu.stat
#├── notify_on_release
#└── tasks			所有采用 cgclassify 绑定到 这个 cgroup 上的进程号


