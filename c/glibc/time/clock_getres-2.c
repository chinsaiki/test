#include <signal.h>
#include <time.h>
#include <stdio.h>

//https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux_for_real_time/7/html/reference_guide/sect-posix_clocks#Using_C_code_to_compare_clock_resolution
//POSIX是用于实现和表示时间源的标准。
//与硬件时钟不同，后者由内核选择并在整个系统中实现；POSIX时钟可由每个应用程序选择，而不会影响系统中的其他应用程序。
//CLOCK_REALTIME：它代表现实世界中的时间，也称为“墙上时间”，表示从墙上的时钟读取的时间。
//    该时钟用于给事件加时间戳，以及与用户进行接口时。可以由具有正确权限的用户修改。
//    但是，应谨慎使用用户修改，因为如果时钟的值在两次读取之间改变，则可能导致错误的数据。
//CLOCK_MONOTONIC：表示系统启动后单调增加的时间。该时钟无法通过任何进程设置，
//    它是计算事件之间时间差的首选时钟。本节中的以下示例CLOCK_MONOTONIC用作POSIX时钟。


//使用clock_getres()比较时钟精度
//使用该clock_getres()功能，您可以检查给定POSIX时钟的分辨率。
//clock_getres()使用与以下两个参数相同的参数clock_gettime()：
//要使用的POSIX时钟的ID，以及指向返回结果的timespec结构的指针。
//以下功能可以比较的精度CLOCK_MONOTONIC和CLOCK_MONOTONIC_COARSE：


int main()
{
    int rc;
    struct timespec res;

	rc = clock_getres(CLOCK_MONOTONIC, &res);
	if (!rc)
		printf("CLOCK_MONOTONIC: %ldns\n", res.tv_nsec); 
	rc = clock_getres(CLOCK_MONOTONIC_COARSE, &res);
	if (!rc)
		printf("CLOCK_MONOTONIC_COARSE: %ldns\n", res.tv_nsec); 
}

