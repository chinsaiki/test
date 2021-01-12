objects_test1 = main.o rte_ring.o 
objects_test2 = test_latency.o rte_ring.o 

CC = gcc

DEFS=

all: $(objects_test1) $(objects_test2)
	$(CC) -g -o main.out $(objects_test1) -lpthread -I. $(DEFS)
	$(CC) -g -o latency.out $(objects_test2) -lpthread -I. $(DEFS)

$(objects_test1): %.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@ -lpthread -I. $(DEFS)

$(objects_test2): %.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@ -lpthread -I. $(DEFS)



clean:
	rm -f  $(objects_test1) $(objects_test2) *.out

