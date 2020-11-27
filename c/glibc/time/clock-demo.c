#include <time.h>
#include <stdio.h>


int main()
{
	clock_t myclock;
    
	printf("CLOCK_PER_SEC:%d, \n", CLOCKS_PER_SEC);

    while(1){
    	myclock = clock();
        
        printf("%d\n", myclock);
        sleep(1); //睡眠不使用 CPU，所以sleep导致这里的输出一直为0
    }
    
	return 0;
}

