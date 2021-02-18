#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#define MAT_W 100
#define MAT_H 100
#define PACK 8

typedef struct matrix{
	int width;
	int height;
	float* elements;
	int num_batch_row;
	__m256* batch_elements;
} matrix_t;

matrix_t make_matrix(int width, int height){
	matrix_t mat;
	mat.width = width;
	mat.height = height;
	int num_elements = width * height;
	mat.num_batch_row = mat.width / PACK;
	if(mat.width % PACK > 0){
		mat.num_batch_row += 1;
	}
	mat.elements = (float*)malloc(sizeof(float) * mat.width * mat.height);
	mat.batch_elements = (__m256*)malloc(sizeof(__m256) * mat.num_batch_row * mat.height);
	return mat;
}

void rand_matrix(matrix_t* mat, int mode){
	srand(time(NULL));
	float base = 100.0;
	int batch_idx = 0;
	int batch = PACK;
	if(mat->width < PACK){
		batch = mat->width;
	}
	for(int i=0;i<mat->height;i++){
		for(int j=0;j<mat->width;j++){
			float scale = rand() / (float) RAND_MAX;
			float value = base * scale;
			if(mode == 0)
				value = i + j + 1;
			mat->elements[i * mat->width + j] = value;

			// Store elements in SSE datatype
			if((j+1) % batch == 0){
				float mat_val[8] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
				float* mat_ptr = &mat->elements[i * mat->width + j - PACK + 1];
				memcpy(&mat_val, mat_ptr, sizeof(float) * PACK);

				__m256 packed_val = _mm256_loadu_ps(mat_val);
				mat->batch_elements[batch_idx] = packed_val;
				batch_idx++;
			}
		}
		int tail = mat->width % PACK;
		if(tail > 0 && batch < mat->width){
			float mat_val[8] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
			float* mat_ptr = &mat->elements[((i + 1) * mat->width) - tail];
			memcpy(&mat_val, mat_ptr, sizeof(float) * tail);

			__m256 packed_val = _mm256_loadu_ps(mat_val);
			mat->batch_elements[batch_idx] = packed_val;
			batch_idx++;
		}
	}
}

static inline float sum256(__m256 val) {
	__m128 hi = _mm256_extractf128_ps(val, 1);
	__m128 lo = _mm256_extractf128_ps(val, 0);
	lo = _mm_add_ps(hi, lo);
	hi = _mm_movehl_ps(hi, lo);
	lo = _mm_add_ps(hi, lo);
	hi = _mm_shuffle_ps(lo, lo, 1);
	lo = _mm_add_ss(hi, lo);
	return _mm_cvtss_f32(lo);
}

void multiply_matrix(matrix_t* mat_a, matrix_t* mat_b, matrix_t* mat_c){
	for(int i=0;i<mat_a->height;i++){
		// printf("perform opeeration on row : %d\n", i);
		for(int j=0;j<mat_b->width;j++){
			register float val = 0.0;
			for(int k=0; k<mat_a->width / PACK; k++){
				__m256 batch_col = _mm256_set_ps(
					mat_b->elements[((k * 8 + 7) * mat_b->width) + j], 
					mat_b->elements[((k * 8 + 6) * mat_b->width) + j], 
					mat_b->elements[((k * 8 + 5) * mat_b->width) + j], 
					mat_b->elements[((k * 8 + 4) * mat_b->width) + j], 
					mat_b->elements[((k * 8 + 3) * mat_b->width) + j], 
					mat_b->elements[((k * 8 + 2) * mat_b->width) + j],
					mat_b->elements[((k * 8 + 1) * mat_b->width) + j], 
					mat_b->elements[((k * 8 + 0) * mat_b->width) + j]);
				__m256 val_batch = _mm256_mul_ps(mat_a->batch_elements[i * mat_a->num_batch_row + k], 
					batch_col);
				val += sum256(val_batch);
			}

			// check the tail
			int tail = mat_a->width % PACK;
			if(tail > 0){
				float* elems = (float*)malloc(sizeof(float) * PACK);
				for(int k=0;k<PACK;k++){
					if(k < tail){
						elems[k] = mat_b->elements[(mat_a->width - tail + k) * mat_b->width + j];
					} else {
						elems[k] = 0.0;
					}
				}
				__m256 batch_col = _mm256_loadu_ps(elems);
				__m256 val_batch = _mm256_mul_ps(mat_a->batch_elements[((i + 1) * mat_a->num_batch_row) - 1], 
					batch_col);
				val += sum256(val_batch);
				free(elems);
			}
			// printf("elem (%d,%d) : %.2f\n", i, j, val);
			mat_c->elements[i * mat_b->width + j] = val;
		}
	}
}

void print_matrix(char* name, matrix_t* mat){
	printf("%s = \n", name);
	for(int i=0;i<mat->height;i++){
		int max_width = 20;
		if(mat->width < max_width){
			max_width = mat->width;
		}
		printf("%s ", "|");
		for(int j=0;j<max_width;j++){
			printf("%.2f ", mat->elements[i * mat->width + j]);
		}
		if(mat->width > max_width){
			printf("... ");
		}
		printf("%s \n", "|");
	}
}

int main(int arg, char** argv){
	struct timeval start_t, end_t;

	long utime;    /* elapsed time in microseconds */
	long mtime;    /* elapsed time in milliseconds */
	long diff_seconds;  /* diff between seconds counter */
	long diff_useconds; /* diff between microseconds counter */

	matrix_t a = make_matrix(MAT_W, MAT_H);
	matrix_t b = make_matrix(MAT_W, MAT_H);
	matrix_t c = make_matrix(MAT_W, MAT_H);

	rand_matrix(&a, 0);
	rand_matrix(&b, 0);

	gettimeofday(&start_t, NULL);
	multiply_matrix(&a, &b, &c);
	gettimeofday(&end_t, NULL);

	// print_matrix("a", &a);
	// print_matrix("b", &b);
	// print_matrix("c", &c);
	
	diff_seconds  = end_t.tv_sec  - start_t.tv_sec;
	diff_useconds = end_t.tv_usec - start_t.tv_usec;
	utime = (diff_seconds) * 1000000 + diff_useconds;
	mtime = ((diff_seconds) * 1000 + diff_useconds/1000.0) + 0.5;

	printf("Matrix multiplication complete, operation time : %ld sec %ld msec %ld usec\n", 
		diff_seconds, mtime, utime);

	return 0;
}