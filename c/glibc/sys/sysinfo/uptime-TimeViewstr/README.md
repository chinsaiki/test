Multi-process, uptime viewer &amp; timer (in C)
============
Author:  Jason Ronquillo

summer 2014

this Filename:  README

Files included:

	- main.c 
	
	- functions.c
	
	- header.h 
	
	- Makefile
	
	- README /* this document */

Compilation instructions:

	Once inside the correct directory, use the commands (in order):
	
		- % make clean
		
		- % make
		
		- % a3 
		
Operation instructions:

	The executable "a3" will run the program with a default of 10 seconds for runtime
	when typed as shown above.  However, the user can adjust how many seconds the 
	program will run simply by typing the desired amount of seconds preceded by a 
	space on the same line as "a3".  For example, to run the program for 15 seconds, 
	just type the following commands (in order):
	
		- % make clean
		
		- % make
		
		- % a3 15

Description:

	This program upon execution will create 3 processes that will perform simultaneously.
	The first process will print once per second the current hour/minute/sec.  The second
	process will print once every 5 seconds the system uptime via the "uptime" excecutable.  
	The third process will serve as a countdown timer, counting down for a default of 10
	seconds - or a user-specified number of seconds - and printing the countdown once per 
	second.  Once the countdown finishes, the third process will send termination signals 
	to the other two child processes.  The parent process prints a farewell message upon
	completion.
	

Notes on design:

	The main() function will call a function (found in the separate 'functions.c' file) 
	that will create the 3 child processes.  Each child process, in turn, will call a 
	corresponding function to perform the actions specified for that particular child 
	process.  There are a total of 4 functions in the 'functions.c' file:
	
		- int createThreeProcesses(int timeData);
		
		- int childProcessOne();
		
		- int childProcessTwo();
		
		- int childProcessThree(int t);
		
	The output is printed directly from each child process, without pipes to the parent 
	process.  I chose to use the execvp() system call for the uptime execution. 
