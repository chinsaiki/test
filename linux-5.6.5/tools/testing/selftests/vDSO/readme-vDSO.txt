Virtual Dynamically-lined Shared Object
这是一个由内核提供的虚拟 .so 文件，他不在磁盘上，而是在内核里。
内核将其映射到一个地址空间，被所有程序共享，正文段大小为一个页面。

因为系统调用的实现从用户态切换到内核态，执行完系统调用后又从内核态切换到
用户态，代价很大，为了加快系统调用的速度，随后先后引入了两种机制

1. vsyscalls
2. VDSO

----------------------------------------
1. test/linux/vDSO
2. test/linux-5.6.5/tools/testing/selftests/vDSO
3. test/linux-5.6.5/tools/testing/selftests/x86	有一些vdso示例
