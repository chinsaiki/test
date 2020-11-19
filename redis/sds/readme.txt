sds(simple dynamic string)

字符串是Redis中最为常见的数据存储类型，其底层实现是简单动态字符串sds(simple dynamic string)，是可以修改的字符串。
它类似于Java中的ArrayList，它采用预分配冗余空间的方式来减少内存的频繁分配。
