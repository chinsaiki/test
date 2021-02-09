/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

//https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux_for_real_time/7/html/tuning_guide/using_coarse_posix_clocks_for_application_timestamping
//
//使用_COARSE POSIX时钟进行应用程序时间戳记
//
//经常执行时间戳记的应用程序会受到读取时钟的成本的影响。用于读取时钟的高成本和时间量可能会对应用程序的性能产生负面影响。
//为了说明这一概念，请想象一下在抽屉内使用时钟来观察事件发生的时间。如果每次必须打开抽屉，获取时钟，然后才读取时间，
//则读取时钟的成本太高，可能会导致丢失事件或为事件加上错误的时间戳。
//相反，墙上的时钟将更快地读取，并且加盖时间戳将对观察到的事件产生较少的干扰。站在该挂钟的正前方可以更快地获取时间读数。
//同样，可以通过选择具有更快读取机制的硬件时钟来获得这种性能增益（降低读取时钟的成本）。
//在Red Hat Enterprise Linux for Real Time中，通过使用具有clock_gettime()以尽可能低的成本产生时钟读数的功能的POSIX时钟，
//可以进一步提高性能。
//
//POSIX时钟
//
//POSIX时钟是用于实现和表示时间源的标准。POSIX时钟可由每个应用程序选择，而不会影响系统中的其他应用程序。
//这与第2.6节“使用硬件时钟进行系统时间戳记”中所述的硬件时钟相反，后者由内核选择并在整个系统中实现。
//
//用于读取给定POSIX时钟的函数为clock_gettime()，该函数在中定义<time.h>。clock_gettime()在内核中具有系统调用形式的副本。
//当用户进程调用时clock_gettime()，相应的C库（glibc）调用sys_clock_gettime()执行所请求的操作的系统调用，然后将结果返回给
//用户程序。
//但是，从用户应用程序到内核的上下文切换需要一定的成本。即使此成本非常低，但如果将操作重复数千次，则累积的成本可能会影响
//应用程序的整体性能。为了避免将该上下文切换到内核，从而使其更快地读取时钟，以VDSO库函数的形式创建了对
//CLOCK_MONOTONIC_COARSE和CLOCK_REALTIME_COARSEPOSIX时钟的支持。
//clock_gettime()使用_COARSE时钟变体之一 通过进行的时间读取不需要内核干预，并且完全在用户空间中执行，从而显着提高了性能。
//_COARSE时钟的时间读数具有毫秒（ms）的分辨率，这意味着小于1ms的时间间隔将不会被记录。_COARSEPOSIX时钟的变体适用于任何可
//以适应毫秒级时钟分辨率的应用，并且在使用硬件时钟且读取成本较高的系统上，其优势更为明显。
//
//注意
//要比较带和不带_COARSE前缀的POSIX时钟读取的成本和分辨率，请参阅《Red Hat Enterprise Linux for Real Time参考指南》。
//示例4.1 在中使用_COARSEClock Variantclock_gettime


//    $ gcc clock_timing.c -o clock_timing -lrt


#include <time.h>

main()
{
	int rc;
	long i;
	struct timespec ts;

	for(i=0; i<10000000; i++) {
		rc = clock_gettime(CLOCK_MONOTONIC_COARSE, &ts);
	}
}

//您可以改进上面的示例，例如通过使用更多字符串来验证的返回码clock_gettime()，验证rc变量的值或确保ts结构的内容受信任。
//该手册clock_gettime()页提供了更多信息，可帮助您编写更可靠的应用程序。

