#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int gVal = 10;

int main()
{
    int lVal = 20;

	/* vfork会阻塞父进程 */
    pid_t pid = vfork();

    if(pid == 0) { //子进程
        lVal++;
        gVal++;
    } else { //父进程
        lVal++;
        gVal++;
    }

    if(pid == 0){
        printf("Child Proc: [%d, %d]\n", gVal, lVal);
        exit(1); //这里需要退出后，父进程才会继续运行
    }
    else {
        wait(NULL);
        printf("Parent Proc: [%d, %d]\n", gVal, lVal);
    }

//    while(1);
    
    return 0;
}
