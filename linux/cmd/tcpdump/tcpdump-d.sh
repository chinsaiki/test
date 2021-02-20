#!/bin/bash
# rongtao 日期 2021年2月20日
# -d 将输入的表达式转化为人类可读的看似汇编的代码

tcpdump -d -i lo tcp and dst port 7070
# 以上指令的输出如下
#(000) ldh      [12]	
#(001) jeq      #0x86dd	         jt 2	jf 6 #检测是否为 IPv6报文，若为假则按照IPv4报文处理
#(002) ldb      [20]
#(003) jeq      #0x6             jt 4	jf 15	# 检测是否为 TCP 报文
#(004) ldh      [56]
#(005) jeq      #0x1b9e          jt 14	jf 15	# 检测目标端口是否 7070(0x1b9e)，若为真，则跳转 L014
#(006) jeq      #0x800           jt 7	jf 15	# 检测是否为 IPv4 报文
#(007) ldb      [23]
#(008) jeq      #0x6             jt 9	jf 15	# 检测是否为 TCP 报文
#(009) ldh      [20]
#(010) jset     #0x1fff          jt 15	jf 11	# 检测是否为 IP 分片(IP Fragmentation)报文
#(011) ldxb     4*([14]&0xf)
#(012) ldh      [x + 16]	# 找到 TCP 报文中 dest port 的所在位置
#(013) jeq      #0x1b9e          jt 14	jf 15	# 检测目标端口是否 7070(0x1b9e)，若为真，则跳转 L014
#(014) ret      #262144		# 该报文符合要求
#(015) ret      #0			# 该报文不符合要求

tcpdump -d -i lo
#(000) ret      #262144		# 该报文符合要求
