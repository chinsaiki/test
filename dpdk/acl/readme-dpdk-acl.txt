DPDK提供了三种classify算法：最长匹配LPM、精确匹配（Exact Match）和通配符匹配（ACL）。

其中的ACL算法，本质是步长为8的Multi-Bit Trie，即每次可匹配一个字节。一般来说步长为n时，
Trie中每个节点的出边为2^n，但DPDK在生成run-time structures时，采用DFA/QRANGE/SINGLE这几
种不同的方式进行数据结构的压缩，有效去除了冗余的出边。本文将为大家介绍ACL算法的基本原理，
主要内容包括：trie树的构造、运行时的node array生成和匹配原理。
对于ACL接口的使用，参考DPDK的官方文档即可。

enum {
	RTE_ACL_FIELD_TYPE_MASK = 0,	：单字节区域如ip头部一个字节的proto字段；
	RTE_ACL_FIELD_TYPE_RANGE,		：采用MASK方式描述，一般对应4字节的源/目的地址；
	RTE_ACL_FIELD_TYPE_BITMASK		：一般对应TCP或UDP头部2字节的PORT区域。
};


