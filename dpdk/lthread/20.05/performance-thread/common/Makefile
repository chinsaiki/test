
INC=-I arch/x86/ \
	-I.
SRCS=lthread.c \
	 lthread_sched.c \
	 lthread_cond.c \
	 lthread_tls.c \
	 lthread_mutex.c \
	 lthread_diag.c \
	 arch/x86/ctx.c \
	 rte_ring.c \
	 rte_timer.c

OUTPUT_LIB=-shared -fPIC -o liblthread.so


all:
	gcc $(SRCS) $(INC) $(OUTPUT_LIB)
clean:
	rm -f liblthread.so
