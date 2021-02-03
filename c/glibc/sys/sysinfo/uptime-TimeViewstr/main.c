/*
Author:  Jason Ronquillo
*/
#include <time.h>
#include <stdio.h>
#include <signal.h>
#include "header.h"

int runtime = 10; /* default value for runtime length in seconds */
//pid_t child1, child2, child3;

int main(int argc, char *argv[]) 
{
	/* if no runtime length is entered by the user at execution, the default of 10 sec is used */
	if(argc == 1) 
	{	
		printf("\nThe runtime for the program will be the default of %d seconds.\n\n", runtime);
	}
	
	/* stores and prints the length entered by the user for the runtime in seconds */
	else 
	{
		runtime = atoi (argv[1]);
		printf("\nThe runtime selected for the program is %d seconds.\n\n",runtime);
	}
	
	/* Creates 3 processes and calls the corresponding function for each process, passing the runtime */
	createThreeProcesses(runtime);
	
	puts("\nEnd of program.  Thank you."); /*May I please have an A for the course, hehe?"*/
	return 0;
} /* End of main() */ 
