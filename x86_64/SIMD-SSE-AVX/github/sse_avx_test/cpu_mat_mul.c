#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define MAT_W 512
#define MAT_H 512

void rand_float(float& mat){
	srand(time(NULL));
	float base = 10.0;
	for(int i=0;i<MAT_H;i++){
		for(int j=0;i<MAT_W;i++){
			float scale = rand() / (float) RAND_MAX;
			mat[i][j] = base * scale;
		}
	}
}

void multiply_matrix(float& a, float& b, float& c){
	for(int i=0;i<MAT_H;i++){
		for(int j=0;i<MAT_W;i++){

		}
	}
}

int main(int arg, char* argv){
	struct timeval start_t, end_t;

	long utime;    /* elapsed time in microseconds */
	long mtime;    /* elapsed time in milliseconds */
	long diff_seconds;  /* diff between seconds counter */
	long diff_useconds; /* diff between microseconds counter */

	float a[MAT_H][MAT_W];
	float b[MAT_H][MAT_W];
	float c[MAT_H][MAT_W];

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