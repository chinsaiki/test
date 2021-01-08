Virtual Dynamically-lined Shared Object
这是一个由内核提供的虚拟 .so 文件，他不在磁盘上，而是在内核里。
内核将其映射到一个地址空间，被所有程序共享，正文段大小为一个页面。

----------------------------------------
1. test/linux/vDSO
2. test/linux-5.6.5/tools/testing/selftests/vDSO
3. test/linux-5.6.5/tools/testing/selftests/x86	有一些vdso示例
