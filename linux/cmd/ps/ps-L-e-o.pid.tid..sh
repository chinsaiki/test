#!/bin/bash
# 荣涛
ps -L -e -o pid,tid,user,state,psr,cmd
#   PID    TID USER     S PSR CMD
#     1      1 root     S   0 /usr/lib/systemd/systemd --switched-root --system --deserialize 21
#     2      2 root     S   1 [kthreadd]
#     3      3 root     I   0 [rcu_gp]
#     4      4 root     I   1 [rcu_par_gp]
				
