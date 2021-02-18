#include<stdio.h>
#include <x86intrin.h>
#define N 128
int main(){
	float a[N]={0};
  	float b[N]={1.1,2.2,3.3};
  	float c[N]={1,2,3};
  	int i, j;
	/*
  	for(i=0;i<N;i++){
    		if(b[i]<c[i])
      			a[i]=b[i]+a[i];
    	else
      		a[i]=c[i]-a[i];
  	}
	*/

	int block = N / 8;
	__m256 ymm_a = _mm256_setzero_ps(); 
	__m256 ymm_b = _mm256_setzero_ps(); 
	__m256 ymm_c = _mm256_setzero_ps();
	__m256 avx_sum, avx_sub;
	__m256 mask;
	__m256 blendv;
	for(i = 0; i < 1; i++){
		ymm_a = _mm256_loadu_ps(a +i*8);
		ymm_b = _mm256_loadu_ps(b +i*8);
		ymm_c = _mm256_loadu_ps(c +i*8);
		
		avx_sum = _mm256_add_ps(ymm_b, ymm_c);
		avx_sub = _mm256_sub_ps(ymm_c, ymm_a);
		mask = _mm256_cmp_ps(ymm_b,ymm_c, 2); //30

		blendv = _mm256_blendv_ps(avx_sub, avx_sum, mask);

		printf("%f, %f, %f, %f, %f, %f, %f, %f\n", 
			blendv[0], blendv[1], blendv[2], blendv[3],
			blendv[4], blendv[5], blendv[6], blendv[7]);
		_mm256_storeu_ps(a + i*8, blendv);
		
	}

  	if(a[2]==3)
    	printf("\n结果正确，测试完成!\n\n");
  	else
    	printf("\n结果不正确，测试完成!\n\n");
  	return 0;
}

