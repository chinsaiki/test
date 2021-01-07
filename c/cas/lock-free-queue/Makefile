SRCS = lockfree_queue.c hazard_ptr.c lockfree_reapd.c

PROGRAMS= test_3_latency

all: $(PROGRAMS)

test_1:$(SRCS)
	gcc -std=c99 -g -m64 -lpthread $^ test_1.c -o test_1.out
	
test_2:$(SRCS)
	gcc -std=c99 -g -m64 -lpthread $^ test_2.c -o test_2.out
	
test_3_latency:$(SRCS)
	gcc -std=c99 -g -m64 -lpthread $^ test_3_latency.c -o test_3_latency.out

clean:
	rm *.out
