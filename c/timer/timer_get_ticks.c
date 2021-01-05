#include <stdio.h>
/**
    从奔腾系列开始，Intel x86 处理器中增加了一个 64 位时间戳寄存器 TSC 
    每经过一个时钟周期，该寄存器+1，及其重启时，该寄存器将清空。

    rdtsc 指一条机器指令，用于读取该时间戳寄存器中的值。

    查询CPU主频：
    
    cat /proc/cpuinfo | grep cpu | grep  MHz | sed -e 's/.*:[^0-9]//'
*/
unsigned long timer_get_ticks(void)
{
    unsigned long ret;
    union
    {
        unsigned long tsc_64;
        struct
        {
            unsigned int lo_32;
            unsigned int hi_32;
        };
    } tsc;

    __asm volatile("rdtsc" :
             "=a" (tsc.lo_32),
             "=d" (tsc.hi_32));

     ret = ((unsigned long)tsc.tsc_64);
     return ret;
}

int main()
{
    unsigned long start = 0;
	while(1) {
        start = timer_get_ticks();
		sleep(1);
		printf("%ld.\n", timer_get_ticks()-start);
	}
}

