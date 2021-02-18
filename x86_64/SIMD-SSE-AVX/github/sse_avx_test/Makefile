CXX=gcc --std=c99
CFLAGS_AVX2=-mavx2
CFLAGS_SSE=-msse
CFLAGS_SSE4=-msse4
LDFLAGS=-lpthread
all: avx_add avx_add_th sse_add sse_mul cpu_add
avx_add:
	$(CXX) $(CFLAGS_AVX2) -o avx_add avx_add.c $(LDFLAGS)
avx_mul:
	$(CXX) $(CFLAGS_AVX2) -o avx_mul avx_mul.c $(LDFLAGS)
cpu_add:
	$(CXX) -o cpu_add cpu_add.c $(LDFLAGS)
avx_add_th:
	$(CXX) $(CFLAGS_AVX2) -o avx_add_th avx_add_th.c $(LDFLAGS)
sse_add:
	$(CXX) $(CFLAGS_SSE) -o sse_add sse_add.c $(LDFLAGS)
sse_mul:
	$(CXX) $(CFLAGS_SSE4) -o sse_mul sse_mul.c $(LDFLAGS)
clean:
	rm -f avx_add avx_add_th sse_add sse_mul cpu_add 
