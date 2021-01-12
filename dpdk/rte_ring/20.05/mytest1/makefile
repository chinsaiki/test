objects = main.o rte_ring.o
CC = gcc

DEFS=

all: $(objects)
	$(CC) -g -o main $(objects) -lpthread -I. $(DEFS)

$(objects): %.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@ -lpthread -I. $(DEFS)

clean:
	rm -f main *.o

