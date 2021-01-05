#include <add.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
	while(1) {
		printf("%d->%d\n", getpid(), add(1,3));
		sleep(1);
	}
}
