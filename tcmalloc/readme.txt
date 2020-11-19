
TCM分配
该存储库包含TCMalloc C ++代码。

TCMalloc是Google对Cmalloc()和C ++ 的定制实现，operator new用于在我们的C和C ++
代码中进行内存分配。TCMalloc是一种快速的多线程malloc实现。


构建TCMalloc
Bazel是TCMalloc的官方构建系统。

《TCMalloc平台指南》包含有关TCMalloc的平台支持的信息。

文献资料
TCMalloc的所有用户应查阅以下文档资源：

该TCMalloc快速入门盖下载，安装，建造和测试TCMalloc，包括你的代码中包含。
该TCMalloc概述涵盖TCMalloc的基本架构，以及如何可能影响配置选择。
《TCMalloc参考》涵盖了C和C ++ TCMalloc API端点。
TCMalloc的更高级用法可能会发现以下文档很有用：

《TCMalloc调整指南》更深入地介绍了配置选择，还说明了自定义TCMalloc的其他方法。
它还涵盖了重要的操作系统级属性，以提高TCMalloc性能。

《TCMalloc设计文档》介绍了TCMalloc在引擎盖下的工作方式，以及做出某些设计选择的原因。
大多数开发人员将不需要此级别的实现细节。

《TCMalloc兼容性指南》记录了我们对API使用方式的期望。

执照
TCMalloc库是根据Apache许可条款获得许可的。有关更多信息，请参见许可。

免责声明：这不是官方支持的Google产品。