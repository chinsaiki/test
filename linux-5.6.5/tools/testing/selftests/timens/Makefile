
all:
	gcc clock_nanosleep.c -pthread -lrt -o clock_nanosleep.out
	gcc exec.c -o exec.out
	gcc gettime_perf.c -ldl -o gettime_perf.out
	gcc procfs.c -o procfs.out
	gcc timens.c -lrt -o timens.out
	gcc timer.c -lrt -o timer.out
	gcc timerfd.c -lrt -o timerfd.out

clean:
	rm -f *.out
