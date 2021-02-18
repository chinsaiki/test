#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

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
	for(int i=0;i<size;i++){
		z[i] = x[i] * y[i];

	}
}

int main(int arg, char* argv){
	struct timeval start_t, end_t;

	long utime;
	long mtime;
	long diff_seconds;
	long diff_useconds;

	float *a,*b,*c;

	a = (float*)malloc(VECTOR_SIZE * sizeof(float));
	b = (float*)malloc(VECTOR_SIZE * sizeof(float));
	c = (float*)malloc(VECTOR_SIZE * sizeof(float));

	rand_float(a);
	rand_float(b);

	gettimeofday(&start_t, NULL);

	multiply_vector(c, a, b, VECTOR_SIZE);

	gettimeofday(&end_t, NULL);
	
	printf("c = ");
	for(int i=0;i<20;i++){
		if(i == 0){
			printf("[");
		} else if(i == 19){
			printf("]\n\n");
		} else if(i == 10){
			printf("... ");
		}
		else {
			printf("%f ", c[i]);
		}
	}

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