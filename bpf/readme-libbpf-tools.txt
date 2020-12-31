BPF 最初代表 Berkeley Packet Filter，但在 Linux 中已扩展为通用内核执行引擎，能够运行新型的用户定义和内核模式应用程序。近几年来，BPF 子系统越发火热，迄至 Linux 5.8 内核，Linux 内核已经支持 30 种 eBPF 程序类型，遍布于内核各个角落，引起了广泛关注及热议，在 LWN 上甚至出现过 Why don't we rename Linux to BPF-kernel？ 这样的嘲讽评论，不过更多的是一些理性的讨论。除上周在 CNCF 和大家分享的 BPF 在 Chaos 领域的应用外，目前 BPF 主要应用于：

1. 跟踪和探测，最有名的应用当属 BCC；
2. 网络功能 (XDP/TC)，用于反 ddos，负载均衡等，K8S 平台有著名的 Cilium；
3. 内核安全策略；
4. 内核回调接口，这种新架构可用于允许 BPF 程序替换内核中的任何 “ops 结构 ( struct xxx_ops ) ，
	社区大佬对此功能的评价是：march towards becoming BPF runtime-powered microkernel，目前 
	TCP 拥塞算法已用此框架实现，感兴趣的可以体验下；
5. 迭代器：dump 内核数据；
