简单的端口转发器

原文
------------------------------------------------------------------
eproxy is a simple epoll based efficient TCP proxy for Linux
It connects two TCP ports to each other and communicates zero copy.

It is not trying to compete with the "big boy" load balancers, 
but is very easy to adapt for experiment. It should be fairly
efficient however.

Simple port forwarder

	proxy inport outip outport

Uses pipes to splice two sockets together. This should give something
approaching zero copy, if the NIC driver is capable. 

This method is rather file descriptor intensive (4 fds/conn), so make sure you 
have enough. 

Andi Kleen


翻译
------------------------------------------------------------------
eproxy是适用于Linux的基于epoll的简单有效TCP代理它将两个TCP端口相互连接，并传递零拷贝。

它不是要与“大男孩”负载平衡器竞争，但很容易适应实验。 应该是公平的效率高。

简单的端口转发器

	proxy inport outip outport

使用管道将两个插座拼接在一起。 这应该给一些东西如果NIC驱动程序有能力，则接近零拷贝。

此方法相当耗费文件描述符（4 fds / conn），因此请确保您有足够的。


安迪·克莱恩


使用方法
------------------------------------------------------------------

打开一个窗口，执行：	nc -l 9999
打开一个窗口，执行： ./eproxy 9998 127.0.0.1 9999
打开一个窗口，执行： telnet 0 9998
