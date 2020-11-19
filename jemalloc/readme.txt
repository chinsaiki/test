GitHub : https://github.com/jemalloc/jemalloc


-------------------------------------------------------------------------------------

自述文件
jemalloc是通用的malloc（3）实现，它强调避免碎片和可扩展的并发支持。 
jemalloc于2005年首次作为FreeBSD 
libc分配器使用，从那时起，它便进入了许多依赖其可预测行为的应用程序。 
2010年，jemalloc的开发工作扩大到包括开发人员支持功能，例如堆分析和广泛的监视/
调整挂钩。现代的jemalloc版本继续被集成回FreeBSD中，因此多功能性仍然至关重要。
正在进行的开发工作趋向于使jemalloc成为各种要求苛刻的应用程序的最佳分配器，并消除/
减轻对实际应用程序有实际影响的弱点。


COPYING文件包含版权和许可信息。

INSTALL文件包含有关如何配置，构建和安装jemalloc的信息。

ChangeLog文件包含每个版本的更改的简短摘要。

网址：http：//jemalloc.net/


-------------------------------------------------------------------------------------

README
jemalloc is a general purpose malloc(3) implementation that emphasizes
fragmentation avoidance and scalable concurrency support.  jemalloc first came
into use as the FreeBSD libc allocator in 2005, and since then it has found its
way into numerous applications that rely on its predictable behavior.  In 2010
jemalloc development efforts broadened to include developer support features
such as heap profiling and extensive monitoring/tuning hooks.  Modern jemalloc
releases continue to be integrated back into FreeBSD, and therefore versatility
remains critical.  Ongoing development efforts trend toward making jemalloc
among the best allocators for a broad range of demanding applications, and
eliminating/mitigating weaknesses that have practical repercussions for real
world applications.

The COPYING file contains copyright and licensing information.

The INSTALL file contains information on how to configure, build, and install
jemalloc.

The ChangeLog file contains a brief summary of changes for each release.

URL: http://jemalloc.net/