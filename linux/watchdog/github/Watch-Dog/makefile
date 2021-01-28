####### Files ##################################################################
SRCS = $(filter-out $(wildcard *_test*) ,$(wildcard *.c))
HDRS = $(wildcard *.h)
OBJECTS = $(patsubst %.c,%.o,$(SRCS))
EXEC = $(patsubst %.c,%.out,$(wildcard *_test.c))
LIBS = $(wildcard ./lib/lib*.a)

####### Compiler ###############################################################
CC = gcc
CFLAGS = -ansi -pedantic-errors -Wall -Wextra -g -Og -fPIC
LDFLAGS = -lm -lpthread
DIR = $(shell pwd)
LIB = libdebug.a

####### All ####################################################################
.PHONY = all
all: libdebug.a
	$(CC) $(CFLAGS) watchdog_shared.c -o watchdog_shared.so  -fPIC -shared -c
	$(CC) $(CFLAGS) watchdog.c -o watchdog.o libdebug.a -lpthread
	$(CC) $(CFLAGS) main_watchdog.c libdebug.a -o main_watchdog.out
	$(CC) $(CFLAGS) watchdog_test.c libdebug.a -lpthread

####### Release ################################################################
.PHONY = release 
release: CFLAGS = -ansi -pedantic-errors -Wall -Wextra -DNDEBUG -O3
release: librelease.a

####### Static Librarry ########################################################
%.a: $(SRCS) $(HDRS) $(OBJECTS)
	ar -rcs $@ $(OBJECTS)

####### Executable #############################################################
%.out: %.o $(LIB)
	$(CC) $(CFLAGS) $< $(LIB) -o $@ $(LDFLAGS)

####### Object #################################################################
%.o: %.d
	$(CC) $< -c -o $@

####### Dependece ##############################################################
%.d: %.c
	$(CC) -MM $< > $@

####### Include ################################################################
.PHONY = include
include:
	ln -sf $(DIR)/*.h $(DIR)/includes

####### Libraries ##############################################################
.PHONY = libs 
libs: $(SRCS) $(HDRS) $(LIBS)
	ln -sf $(DIR)/*.a $(DIR)/lib

####### Test ###################################################################
.PHONY = test
test: $(EXEC)
	ln -sf $(DIR)/*.out $(DIR)/test

####### Clean ##################################################################
.PHONY = clean
clean:
	find . -name '*.out' -delete
	find . -name '*.o' -delete
	find . -name '*.so' -delete
	find . -name '*.a' -delete
	find . -name '*.d' -delete

#gd watchdog_shared.c -o watchdog_shared.so  -fPIC -shared -c 

#gd watchdog_test.c watchdog.o watchdog_shared.so libdebug.a -lpthread

#gd watchdog.c -o watchdog.o watchdog_shared.so libdebug.a -lpthread

#gd main_watchdog.c watchdog_shared.so libdebug.a 
