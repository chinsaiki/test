#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <immintrin.h>
#include <stdbool.h>

#include <string.h>
#include <time.h>

float c[9] = {0.299, 0.587, 0.114, -0.147, -0.289, 0.436, 0.615, -0.515, -0.100};
//float c[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

int rgb_yuv_c(float * rgb, float * yuv, int n, int m)
{

  for (int i = 0; i < n*m; i += 3)
  {
    // Y
    float y = c[0] * rgb[i] + c[1] * rgb[i+1] + c[2] * rgb[i+2];
    // U
    float u = c[3] * rgb[i] + c[4] * rgb[i+1] + c[5] * rgb[i+2] + 128;
    // V
    float v = c[6] * rgb[i] + c[7] * rgb[i+1] + c[8] * rgb[i+2] + 128;

    if (v < 0)
      v = 0;
    if (v > 255)
      v = 255;


    yuv[i] = y;
    yuv[i+1] = u;
    yuv[i+2] = v;
    i++;
  }
  return 0;
}

int rgb_yuv_avx_v2(float * m2, int c, int f, float * m3)
{

  __m256 comp0 = _mm256_set_ps(0,0,0,0,0,0,0,0);
  __m256 comp255 = _mm256_set_ps(255,255,255,255,255,255,255,255);
  __m256 y = _mm256_set_ps(0, 0.615, -0.147, 0.299, 0, 0.615, -0.147, 0.299);
  __m256 u = _mm256_set_ps(0, -0.515, -0.289, 0.587, 0, -0.515, -0.289, 0.587);
  __m256 v = _mm256_set_ps(0, -0.100, 0.436, 0.114, 0, -0.100, 0.436,  0.114);
  __m256 uv128 = _mm256_set_ps(0,128,128,0,0,128,128,0);

  __m256 rr,rb,rg,mul1r,mul2r,mul3r,sumr,g126r,condr,condb;
  int indice = 0;

  for (int i = 0; i < f ; i++){
    indice = i*c;
      for (int j = 0; j<c/8; j++){
      rr = _mm256_set_ps(0,m2[indice+4],m2[indice+4],m2[indice +4],0, m2[indice],m2[indice], m2[indice]);
      rg = _mm256_set_ps(0,m2[indice+4+1],m2[indice+1+4],m2[indice+1+4],0, m2[indice+1],m2[indice+1], m2[indice+1]);
      rb = _mm256_set_ps(0,m2[indice+4+2],m2[indice+4+2],m2[indice+2+4],0, m2[indice+2],m2[indice+2], m2[indice+2]);
      mul1r = _mm256_mul_ps(y, rr);
      mul2r = _mm256_mul_ps(u, rg);
      mul3r = _mm256_mul_ps(v, rb);
      sumr = _mm256_add_ps(mul1r, mul2r);
      sumr = _mm256_add_ps(sumr, mul3r);
      g126r = _mm256_add_ps(uv128,sumr);
      condr = _mm256_min_ps(g126r,comp255);
      condb = _mm256_max_ps(condr,comp0);
      _mm256_storeu_ps(&m3[indice], condb);
      indice += 8;
    }
  }
  return 0;
}


void print_matrix3(int m, int n, float * matrix)
{
  printf("Resultados: (*)\n");
  int cont = 3;
  for (int i = 0; i<(n*m)-1; i++)
  {
    if (i == cont && i <n*m){
      i++;
      cont = cont + 4;

    }
    if (i%n == 0)
       printf("\n");

    printf(" %d", (int)matrix[i]);
  }
  printf("\n");
}


/* Calcula numeros pseudo-aleatorios y los inserta en cada posicion de la matriz*/
void random_matrix_n(int n, int m, float * matrix)
{
  int i, j;
  int psrand = 1;

  srand(time(NULL));

  // CALCULO DE NUMEROS

  for (i = 0; i<n; i++)
  {
    for (j = 0; j<m; j++)
    {
      //int value = psrand++;
      int value = rand()%255;
      matrix[i*m+j] = value;
    }

  }


}

void random_matrix_0(int n, int m, float * matrix)
{
  int i, j, t;
  int psrand = 1;

  srand(time(NULL));

  // CALCULO DE NUMEROS
int cont = 3;
  for (i = 0; i<n; i++)
  {
    for (j = 0; j<m; j++)
    {
     if (j == cont){

      matrix[i*m+j] = 0;
      cont = cont + 4;
    }else{

      //int value = psrand++;
      int value = rand()%255;
      matrix[i*m+j] = value;
    }
    }
    cont = 3;
  }


}

void random255(int n, int m, float * matrix)
{
  for (int i = 0; i<n*m; i++)
  {
    if (i == 0 || (i%4) == 0)
      matrix[i] = 255;
    else matrix[i] = 0;


  }
}

int main(int argc, char *argv[])
{

  char opt = 'N';
  int n = 0;
  int m = 0;
  puts("Introduzca n: ");
  scanf("%d", &n);
  puts("Introduzca m: ");
  scanf("%d", &m);
  getchar();
  puts("Mostrar datos por pantalla (S/N): ");
  opt = getchar();

  int f = n;
  int c = m + (m/3);

  float * m2 = NULL;
  if ((m2 = malloc(sizeof(float*)*f*c)) == NULL)
  {
    perror("malloc error");
    return -1;
  }


  float * m3 = NULL;
  if ((m3 = malloc(sizeof(float*)*f*c)) == NULL)
  {
    perror("malloc error");
    return -1;
  }


  struct timeval iteInicio, iteFinal;
  double tiempo_c, tiempo_avx, tiempo_avx_reoder, tiempo_avx_antonio;

random_matrix_0(c, f, m2);

if (opt == 's' || opt == 'S') print_matrix3(f, c, m2);

gettimeofday(&iteInicio, NULL);
rgb_yuv_c(m2, m3, c, f);		// T
gettimeofday(&iteFinal, NULL);

tiempo_c = ((double)iteFinal.tv_sec + (double)iteFinal.tv_usec/1000000) -
      ((double)iteInicio.tv_sec + (double)iteInicio.tv_usec/1000000);

printf("\n");
printf("Tiempo en C: %f\n", tiempo_c);


if (opt == 's' || opt == 'S') print_matrix3(f, c, m3);


gettimeofday(&iteInicio, NULL);
rgb_yuv_avx_v2(m2,c,f,m3);		// T
gettimeofday(&iteFinal, NULL);

tiempo_avx_antonio = ((double)iteFinal.tv_sec + (double)iteFinal.tv_usec/1000000) -
      ((double)iteInicio.tv_sec + (double)iteInicio.tv_usec/1000000);

printf("\n");
printf("Tiempo en AVX (V2): %f\n", tiempo_avx_antonio);

if (opt == 's' || opt == 'S') print_matrix3(f, c, m3);


free(m2);
free(m3);


  return 0;
}

// gcc -mavx ejer2.c -o ejer2
