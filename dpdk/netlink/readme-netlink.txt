NETLINK (AF_NETLINK | PF_NETLINK)
Netlink套接字是用以实现用户进程和内核进程通信的一种特殊的进程间通信，
也是网络应用程序与内核通信的最常用的接口。

NETLINK_ROUTE	路由
NETLINK_USERSOCK	用户态 socket 协议
NETLINK_FIREWALL	用户态防火墙
NETLINK_NETFILTER	netfilter子系统
NETLINK_KOBJECT_UEVENT	内核事件向用户态通知
NETLINK_GENERIC	通用netlink

实现与库 和 驱动等
-------------------------------
1. test/linux/netlink
2. test/tcp-ip/netlink
3. test/dpdk/netlink

头文件
-------------------------------
1. test/c/glibc/netlink/
2. test/c/glibc/linux/netlink/

示例代码
-------------------------------
1. test/linux-5.6.5/tools/testing/selftests/uevent
