objects = main.o rte_ring.o
CC = gcc

DEFS=-DALLOW_EXPERIMENTAL_API

all: $(objects)
	$(CC) -g -o main $(objects) -lpthread -I. $(DEFS)

$(objects): %.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@ -I. $(DEFS)

clean:
	rm -f main *.o

