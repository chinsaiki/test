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

void multiply_vector(float* z, float* x , float* y, int size){
	for(int i=0;i<size / 8;i++){
		z[i] = x[i] * y[i] + x[i] + y[i];
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

typedef struct thread_args{
	float* a;
	float* b;
	float* c;
	int size;
	int offset;
}thread_args;

void multiply_vector_th(void *ptr){
	thread_args* args = ptr;
	float* a = args->a;
	float* b = args->b;
	float* c = args->c;
	int size = args->size;
	int offset = args->offset;
	for(int i=offset;i<size / 8;i++){
		__m256 _vec_a = _mm256_loadu_ps(a + i * 8);
		__m256 _vec_b = _mm256_loadu_ps(b + i * 8);
		__m256 _vec_c = _mm256_mul_ps(_vec_a, _vec_b);
		memcpy(c + (i*8), &_vec_c, 8 * sizeof(float));
	}

	int tail = (size % 8);
	if(tail != 0){
		float tail_a[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
		float tail_b[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
		memcpy(&tail_a, a + offset + (size - tail), tail);
		memcpy(&tail_b, b + offset + (size - tail), tail);
		__m256 _vec_t_a = _mm256_loadu_ps(tail_a);
		__m256 _vec_t_b = _mm256_loadu_ps(tail_b);
		__m256 _vec_c = _mm256_mul_ps(_vec_t_a, _vec_t_b);
		memcpy(c + size - tail + offset, &_vec_c, tail * sizeof(float));
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

	rand_float(a);
	rand_float(b);

	gettimeofday(&start_t, NULL);

	int num_t = 4;
	pthread_t threads[num_t];
	int size = VECTOR_SIZE / num_t;
	printf("Creating thread execution ...\n");
	for(int i=0;i<num_t;i++){
		int data_length = size;
		if(i == num_t - 1){
			int tail = VECTOR_SIZE % num_t;
			if(tail > 0){
				data_length += tail;
			}
		}
		thread_args args = {a,b,c,data_length,i*size};
		int ret = pthread_create(&threads[i], NULL, &multiply_vector_th, (void*)&args);
		if(ret){
			printf("Thread creation failed. Exiting.\n");
			exit(1);
		}
		pthread_join(threads[i], NULL);
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