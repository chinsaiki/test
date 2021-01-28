# Watch-Dog
The purpose of the project is to keep a program in a user-critical section, that could be a situation that the program will fall in that section and revive the program.

i made MakeFile for it, but i'll leave here instructions(in LINUX):

1-gd watchdog_shared.c -o watchdog_shared.so -fPIC -shared -c

2-gd watchdog_test.c watchdog.o watchdog_shared.so libdebug.a -lpthread

3-gd watchdog.c -o watchdog.o watchdog_shared.so libdebug.a -lpthread

4-gd main_watchdog.c watchdog_shared.so libdebug.a
