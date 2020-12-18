#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

int gVal = 10;

int main()
{
    int lVal = 20;

	/* vfork会阻塞父进程 */
    pid_t pid = vfork();

    if(pid == 0){
        lVal++;
    }
    else{
        gVal++;
    }

    if(pid == 0){
        printf("Child Proc: [%d, %d]\n", gVal, lVal);
    }
    else
        printf("Parent Proc: [%d, %d]\n", gVal, lVal);

    while(1);
    
    return 0;
}
