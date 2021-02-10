#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
	int i;
	struct timespec ts;

	for(i=0; i<5; i++) {
#ifndef     COARSE    
		clock_gettime(CLOCK_MONOTONIC, &ts);
#else        
        //所述_COARSE时钟具有一个一毫秒的精度，因此只的前三个数字tv_nsec的timespec结构的场是显著。
        clock_gettime(CLOCK_MONOTONIC_COARSE, &ts);
#endif    
		printf("%ld.%ld\n", ts.tv_sec, ts.tv_nsec);
		usleep(200);
	}
}

