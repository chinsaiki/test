/*
Author:  Jason Ronquillo
*/
#include <time.h>
#include <stdio.h>
#include <signal.h>
#include "header.h"

pid_t child1, child2, child3;

int createThreeProcesses(int timeData)
{
	/* for first child process */
	if (!(child1 = fork())) 
	{
		childProcessOne();
		exit(0);
	} 
	/* for second child process */
	else if (!(child2 = fork())) 
	{
		childProcessTwo();
		exit(0);
	}
	/* for third child process */
	else if (!(child3 = fork())) 
	{
		/* passing the runtime data */
		childProcessThree(timeData);
		exit(0);
	}
	/* for the parent process */
	else 
	{
		/* busy-wait for all child processes to finish */
		wait(&child1);
		wait(&child2);
		wait(&child3);
	}
} /* end */

/* This function is performed by child process #1.
It will print the current time once every second,
and loop indefinitely until the termination signal 
is sent by child process #3. 
The following link was used as a resource:
http://labs.hoffmanlabs.com/node/1277 */
int childProcessOne()
{
	int go = 0;
	time_t epoch_time;
	struct tm *tm_p;
	
	while(go == 0) /* loops indefinitely, or until termination signal */
	{
		
		
		epoch_time = time( NULL );
		tm_p = localtime( &epoch_time );
		
		/* this will print the formatted current time */
		printf("The current time is %.2d:%.2d:%.2d\n", 
			tm_p->tm_hour, tm_p->tm_min, tm_p->tm_sec );
		/* once every second */	
		sleep(1);
	}
} /* end */

/* Performed by child process #2.
This will fork/execvp every 5 seconds until
the termination signal is sent by child process #3. 
The following link was used as a resource:
http://web.eecs.utk.edu/~plank/plank/classes/cs360/360/notes/Exec/lecture.html */
int childProcessTwo()
{
	char *uptime[1];
	int i, j = 0;
	
	/* contains the executable */
	uptime[0] = "/usr/bin/uptime";
	
	/* loops indefinitely */
	while(j == 0)
	{
		/* creates a fork/execvp for the executable */
		if (fork() == 0)
		{
			i = execvp("/usr/bin/uptime", uptime);
    		}
		/* once every 5 seconds */
		sleep(5);
	}
} /* end */

/* performed by child process #3 */ 
int childProcessThree(int t)
{
	int time, min, sec;
	
	/* this will loop for the number of seconds specified at execution
	   or the default of 10 seconds */
	for(time = t; time != 0; time--)
	{
		
		if(time > 59)
		{
			/* formatting the timer output */
			min = time/60;
			sec = time%60;
			printf("Countdown - %02d:%02d\n", min, sec);
		}
		else
			printf("Countdown - 00:%02d\n", time);
			
		sleep(1);
	}
	
	puts("\nFinishing countdown - time's up!");
	
	/* sending termination signal to child process 1 */
	kill(child1, SIGTERM);
	puts("\nTerminating Process 1.");
	
	/* sending termination signal to child process 2 */
	kill(child2, SIGTERM);
	puts("Terminating Process 2.");
	
} /* end */
