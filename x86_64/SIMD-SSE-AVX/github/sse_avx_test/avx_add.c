#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#define VECTOR_SIZE (2048 * 2048)

void rand_float(float * vec){
	srand(time(NULL));
	float base = 10.0;
	for(int i=0;i<VECTOR_SIZE;i++){
		float scale = rand() / (float) RAND_MAX;
		vec[i] = base * scale;
	}
}

void print_sample_vector(char* name, float* vec){
	printf("%s = ", name);
	for(int i=0;i<20;i++){
		if(i == 0){
			printf("[");
		} else if(i == 19){
			printf("]\n\n");
		} else if(i == 10){
			printf("... ");
		}
		else {
			printf("%f ", vec[i]);
		}
	}
}

int main(int arg, char* argv){
	struct timeval start_t, end_t;

	long utime;    /* elapsed time in microseconds */
	long mtime;    /* elapsed time in milliseconds */
	long diff_seconds;  /* diff between seconds counter */
	long diff_useconds; /* diff between microseconds counter */

	float *a,*b,*c;

	a = (float*)malloc(VECTOR_SIZE * sizeof(float));
	b = (float*)malloc(VECTOR_SIZE * sizeof(float));
	c = (float*)malloc(VECTOR_SIZE * sizeof(float));
	// posix_memalign((void **)&a, 32, VECTOR_SIZE * sizeof(float));
	// posix_memalign((void **)&b, 32, VECTOR_SIZE * sizeof(float));
	// posix_memalign((void **)&c, 32, VECTOR_SIZE * sizeof(float));

	rand_float(a);
	rand_float(b);

	gettimeofday(&start_t, NULL);

	for(int i=0;i<VECTOR_SIZE / 8;i++){
		__m256 _vec_a = _mm256_loadu_ps(a + i * 8);
		__m256 _vec_b = _mm256_loadu_ps(b + i * 8);
		__m256 _vec_c = _mm256_mul_ps(_vec_a, _vec_b);
		memcpy(c + (i*8), &_vec_c, 8 * sizeof(float));
	}

	int tail = VECTOR_SIZE % 8;
	if(tail != 0){
		float tail_a[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
		float tail_b[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
		memcpy(&tail_a, a + (VECTOR_SIZE - tail), tail);
		memcpy(&tail_b, b + (VECTOR_SIZE - tail), tail);
		__m256 _vec_t_a = _mm256_loadu_ps(tail_a);
		__m256 _vec_t_b = _mm256_loadu_ps(tail_b);
		__m256 _vec_c = _mm256_mul_ps(_vec_t_a, _vec_t_b);
		memcpy(c + VECTOR_SIZE - tail, &_vec_c, tail * sizeof(float));
	}
	gettimeofday(&end_t, NULL);

	print_sample_vector("a", a);
	print_sample_vector("b", b);
	print_sample_vector("c", c);
	
	diff_seconds  = end_t.tv_sec  - start_t.tv_sec;
	diff_useconds = end_t.tv_usec - start_t.tv_usec;
	utime = (diff_seconds) * 1000000 + diff_useconds;
	mtime = ((diff_seconds) * 1000 + diff_useconds/1000.0) + 0.5;

	printf("vector additon complete, operation time : %ld sec %ld msec %ld usec\n", 
		diff_seconds, mtime, utime);
	free(a);
	free(b);
	free(c);

	return 0;
}