#include <signal.h>
#include <stdio.h>

void sig_handler(int signum)
{
	printf("get signal.\n");
	exit(0);
}

int main(int argc, char*argv[])
{
	printf("NSIG = %d\n", NSIG);
	printf("SIGFPE = %d\n", SIGFPE);

	signal(SIGFPE, sig_handler);

	int a = argv[1]?atoi(argv[1]):0;
	int b = 1/a;

	return 0;
}
