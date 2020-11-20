GCC=gcc
CFLAGS=-Wall -fPIC -shared
LDFLAGS=-ldl
SOURCE=libpreload.c
TARGET=libpreload.so
STRIP=/usr/bin/strip

all: 
	$(GCC) $(CFLAGS) $(SOURCE) -o $(TARGET) $(LDFLAGS)
	$(GCC) $(CFLAGS) test.c -o test.out

clean:
	rm $(TARGET) *.out
