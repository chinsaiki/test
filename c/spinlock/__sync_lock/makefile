all:
	gcc -DMUTEX -o mutex.out test.c -pthread
	gcc -o spin.out test.c -pthread
	gcc -DNONLOCK -o nonlock.out test.c -pthread

clean:
	rm -f *.out
