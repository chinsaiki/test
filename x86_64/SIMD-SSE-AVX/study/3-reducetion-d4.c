#include<stdio.h>
#define N 128
#include <x86intrin.h>


int main()
{
	float sum = 1;
    	float a[N]={1,2,3};
    	int i;
	/*
    	for(i=0;i<N;i++)
        	sum+=a[i];
	*/
	__m256 avx_sum = _mm256_setzero_ps();
	__m256 ymm0;
	int block = N / 8;
	
	for(i = 0; i < block; i++){
		ymm0 = _mm256_loadu_ps(a + i*8);
		avx_sum = _mm256_hadd_ps(ymm0, avx_sum);
	}
	
	avx_sum = _mm256_hadd_ps(avx_sum, avx_sum);
	avx_sum = _mm256_hadd_ps(avx_sum, avx_sum);
	
	sum += avx_sum[0] + avx_sum[4];

	printf("sum = %f ", sum);
	if(sum==7)
        	printf("\n结果正确，测试完成!\n\n");
	else
       		printf("\n结果不正确，测试完成!\n\n");
    
    return 0;
}

