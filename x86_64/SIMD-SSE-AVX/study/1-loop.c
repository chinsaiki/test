#include<stdio.h>
#include <x86intrin.h>
#define N 128

int main(){
  	double a[N][N],b[N][N],c[N][N];
  	int i,j;

  	for(i=0;i<N;i++)
    		for(j=0;j<N;j++)
			{
     			 a[i][j]=10;
     			 b[i][j]=6;
    		}
/*
	for(i = 0; i < N; i++){
		for(j = 0; j < N; j++)
			c[i][j] = a[i][j] + b[i][j];
	}
*/
	int block = N / 4;
	int reserve = N % 4;
	__m256d ymm0, ymm1;
	__m256d avx_sum0 = _mm256_setzero_pd();
	for(i = 0; i < N; i++){
		for(j = 0; j < block; j ++){
			ymm0 = _mm256_loadu_pd(&a[i][j*4]);
			ymm1 = _mm256_loadu_pd(&b[i][j*4]);
			avx_sum0 = _mm256_add_pd(ymm0, ymm1);
			
			_mm256_storeu_pd(*(c+i)+j*4, avx_sum0);
		}
	}
 	if(c[4][6]==16)
		printf("\n结果正确，测试完成!\n\n");
	else
    		
		printf("\n结果不正确，测试完成!\n\n");
  	return 0;
}

