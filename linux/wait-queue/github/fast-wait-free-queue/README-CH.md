快速等待免费队列
这是用于评估并发队列性能的基准框架。当前，它包含四个并发队列实现。他们是：

快速的免等待队列wfqueue，
莫里森和Afek的lcrq，
Fatourou和Kallimanis的ccqueue，并
迈克尔和斯科特的 msqueue
基准框架还包括一个综合队列基准，faa该基准使用fetch-and-add原语模拟入队和出队，以测试系统的性能fetch-and-add。

该框架当前包含一个基准，pairwise其中所有线程重复执行成对入队和出队操作。在两个操作之间，pairwise使用一个延迟例程，该例程添加一个任意延迟（在50〜150ns之间），以避免人为的长期运行情况，在这种情况下，一个线程长时间保持高速缓存行。

要求
GCC 4.1.0或更高版本（推荐GCC 4.7.3或更高版本）：当前实现使用GCC__atomic或__sync原语进行原子内存访问。
Linux内核2.5.8或更高版本
glibc 2.3：我们用于sched_setaffinity将线程绑定到核心。
原子CAS2：lcrqrequire CAS2，一个16字节宽的compare-and-swap原语。在最新的Intel处理器和IBM Power8上可用。
jemalloc（可选）：jemalloc消除了内存分配器的瓶颈。您可以jemalloc通过将JEMALLOC_PATH环境变量设置jemalloc为安装路径来进行链接。
如何安装
下载已发布的源代码tarball之一，然后执行以下命令。使用的文件名可能会有所不同，具体取决于您下载的tarball的名称。

$ tar zxf fast-wait-free-queue-1.0.0.tar.gz
$ cd fast-wait-free-queue-1.0.0
$ make
这应该产生6个二进制文件（或5，如果你的系统不支持CAS2，lcrq将无法编译）： ，wfqueue，wfqueue0，lcrq，ccqueue，msqueue，faa和delay。这些是pairwise使用不同队列实现编译的基准。

wfqueue0：与相同，wfqueue只是将其PATIENCE设置为0。
delay：用于衡量延迟例程所花费时间的综合基准。
怎么跑
您可以使用线程数作为参数直接执行二进制文件。不带参数的情况下，执行将使用系统上所有可用的内核。

例如，

./wfqueue 8
运行wfqueue8个线程。

如果要验证结果，请使用编译二进制文件VERIFY=1 make。然后直接执行二进制文件将打印PASSED或错误消息。

您还可以使用driver脚本，该脚本最多调用10次二进制文件，并测量每次运行的平均运行时间，当前运行的运行时间，标准偏差，误差范围（时间和百分比）。当误差幅度相对较小（<0.02）或已调用二进制文件10次时，脚本将终止。

例如，

./driver ./wfqueue 8
wfqueue使用8个线程最多运行10次​​，并收集统计结果。

您可以使用benchmark脚本，该脚本driver在二进制列表和线程数列表的所有组合上调用，mean running time并margin of error为每个组合报告和。您可以使用环境变量指定二进制文件列表TESTS。您可以使用环境变量指定线程数列表PROCS。

的生成的输出benchmark可用作gnuplot的数据文件。benchmark的输出的第一列是线程数。然后，每两列分别是mean running time和margin of error用于每个队列的实现。它们的顺序与中指定的顺序相同TESTS。

例如，

TESTS=wfqueue:lcrq:faa:delay PROCS=1:2:4:8 ./benchmark
运行每个的wfqueue，lcrq，faa，和delay使用1-，2，4和8的螺纹。

然后您可以使用

set logscale x 2
plot "t" using 1:(20000/($2-$8)) t "wfqueue" w lines, \
     "t" using 1:(20000/($4-$8)) t "lcrq" w lines, \
     "t" using 1:(20000/($6-$8)) t "faa" w lines
How to map threads to cores
By default, the framework will map a thread with id i to the core with id i % p, where p is the number of available cores on a system; you can check each core's id in proc/cpuinfo.

To implement a custom mapping, you can add a cpumap function in cpumap.h. The signature of cpumap is

int cpumap(int id, int nprocs)
where id is the id of the current thread, nprocs is the number of threads. cpumap should return the corresponding core id for the thread. cpumap.h contains several examples of the cpumap function. You should guard the definition of the added cpumap using a conditional macro, and add the macro to CFLAGS in the makefile.

How to add a new queue implementation
We use a generic pointer void * to represent a value that can be stored in the queue. A queue should implements the queue interface, defined in queue.h.

queue_t: the struct type of the queue,
handle_t: a thread's handle to the queue, used to store thread local state,
void queue_init(queue_t * q, int nprocs): initialize a queue; this will be called only once,
void queue_register(queue_t * q, handle_t * th, int id): initialize a thread's handle; this will be called by every thread that uses the queue,
void enqueue(queue_t * q, handle_t * th, void * val): enqueues a value,
void * dequeue(queue_t * q, handle_t * th): dequeues a value,
void queue_free(queue_t * q, handle_t * h): deallocate a queue and cleanup all resources associated with it,
EMPTY: a value that will be returned if a dequeue fails. This should be a macro that is defined in the header file.
How to add a new benchmark
A benchmark should implement the benchmark interface, defined in benchmark.h, and interact with a queue using the queue interface. The benchmark interface includes:

void init(int nprocs, int n): performs initialization of the benchmark; called only once at the beginning.
void thread_init(int id, int nprocs): performs thread local initialization of the benchmark; called once per thread, after init but before benchmark.
void * benchmark(int id, int nprocs): run the benchmark once, called by each thread to run the benchmark. Each call will be timed and report as one iteration. It can return a result, which will be passed to verify to verify correctness.
int verify(int nprocs, void * results): should verify the result of each thread and return 0 on success and non-zero values on error.

