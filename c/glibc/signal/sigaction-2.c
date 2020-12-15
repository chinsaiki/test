#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handler(int signum)
{
	printf("Catch-C .\n");	
	exit(1);
}

int main()
{
	struct sigaction sa;
	sa.sa_handler = handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);

	sigaction(SIGINT, &sa, NULL);

	while(1) {
		printf("Running...\n");
		sleep(1);
	}
}
