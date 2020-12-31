all: wait-wake.out requeue.out

wait-wake.out: wait-wake.o
	$(CC) $^ -lpthread -o $@

requeue.out: requeue.o
	$(CC) $^ -lpthread -o $@

clean:
	rm -f *.o *.out

