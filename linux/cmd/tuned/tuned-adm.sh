#!/bin/bash
# https://blog.csdn.net/mx_steve/article/details/100558608

# rhel6中引入了tuned调优策略，rhel7中内容了10种调优方案。它提供了三种调优方法。

# 方法1：使用静态调优方案
tuned-adm list # 列出所有的默认的调优策略
tuned-adm active # 查看现在生效的调优策略
tunde-adm recommand # 查看tuned目前推荐使用的策略
tuned-adm profile 方案名称  # 修改调优方案，使用新的调优策略
tuned-adm off   # 关闭调优策略

# 方法2：使用动态调优策略
# tuned内置的十种调优策略中，可能不太适用，需要实时修改调优策略。

#vim /etc/tuned/tuned-main.conf
# dynamic_tunning=1  # 启动动态调优
# update_interval=10 # 每次调优的时间间隔
#systemctl restart tuned
#systemctl enable tuned
#重启后生效，适用于那些需要实时修改调优策略的方法。

# 方法3：适用自定义调优策略
# 十种方案在有些情况可能不适用，需要我们自定义进行修改默认策略，更适用于我们现实任务。
#/usr/bin/tuned是我们自定义的调优方案存放的位置
#/etc/tuned也可以让我们做自定义配置
#/etc/tuned/active_profile 可以看到当前的调优方案
#/etc/tuned/tuned-main.conf 是当前的tuned的主配置方案

# 自定义调优配置方案：
# mkdir /etc/tuned/myprofile
# vim /etc/tuned/myprofile/tuned.conf
#     [main]
#     include=powersave
#  
#     [NAME]
#     type=插件名称      # 此处可以是： cpu,disk,mount,net,vm,sysctl,script等
#     devices=设备名字   # 对应设备的名字
#     elevator=deadline #io调度方案
# tuned-adm profile myprofile
# tuned-adm active
# tuned-adm list
# 如果使用插件名作为NAME，type可以忽略，以上三种调优方案可以按需选择。
