#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <immintrin.h>
#include <string.h>
#include <stdint.h>

/* Funcion que recibe una matriz nxm como entrada para devolver otra matriz traspuesta
M = M^t */
void transpose(int n, int m, float * matrix, float * output)
{
  int i;
  int index = 0;
  for (int j = 0; j<m; j++)
  {
    for (i = j; i < n*m; i +=m)
    {
      output[index] = matrix[i];
      index++;
    }

  }

}

/* Calcula el total de cada columna de una matriz t_matrix
y devuelve el total en un array, versión C */
void ftotal(int n, int m, float * t_matrix, float * total)
{
  int index = 0;
  int t = 0;
  // TOTAL
  for (int i = 0; i < n*m; i+=n)
  {
    for (int j = i; j < i+n; j++)
      t += t_matrix[j];

    total[index] = t;
    index++;
    t = 0;
  }
}

/* Calcula el total de cada columna de una matriz t_matrix
y devuelve el total en un array, versión AVX */
void total_avx(int n, int m, float * t_matrix, float * total)
{

  int8_t mbits[8];
  __m256 zero = _mm256_setzero_ps();
  __m256 sum = _mm256_setzero_ps();
  __m256 t = _mm256_setzero_ps();

  __m256i mask1 = _mm256_setr_epi32(-1,-1,-1,-1,0,0,0,0);
  __m256i mask2 = _mm256_setr_epi32(-1,0,0,0,0,0,0,0);

  int iteraciones = n/8;
  int resto = 0;

  for (int col = 0; col < m; col++)
  {
    resto = n%8;
    for (int i = 0; i<iteraciones; i++)
    {
      __m256 p1 = _mm256_maskload_ps(t_matrix, mask1);
      __m256 p2 = _mm256_maskload_ps(t_matrix+4, mask1);
      sum = _mm256_hadd_ps(p1, p2);
      sum = _mm256_hadd_ps(sum, zero);
      sum = _mm256_hadd_ps(sum, zero);
      t = _mm256_add_ps(t, sum);
      t_matrix += 8;
    }

    while (resto > 0)
    {
      __m256 a = _mm256_maskload_ps(t_matrix, mask2);
      t_matrix++;
      t = _mm256_add_ps(t, a);
      resto--;
    }
    _mm256_maskstore_ps(&total[col], mask2, t);
    t = _mm256_setzero_ps();
  }
}

/* Calcula numeros pseudo-aleatorios y los inserta en cada posicion de la matriz */
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
      int value = rand()%100000+1;
      matrix[i*m+j] = value;
    }

  }


}

/* Imprime la matriz y el total a partir de los dos array's */
void print_matrix2(int n, int m, float * matrix, float * total)
{
  printf("Resultados: (*)\n");
  int i, j;
  for (i = 0; i<n; i++)
  {
    printf("\t");
    for (j = 0; j<m; j++)
      printf("\t%d ", (int)matrix[i*m+j]);

    printf("\n");
  }
  printf("\n");
  printf("TOTAL (*): ");
  for (int i = 0; i<m; i++ )
    printf("\t%d ", (int)total[i]);

printf("\n");
}



int main(int argc, char *argv[])
{

  struct timeval iteInicio, iteFinal;
  double tiempo_c, tiempo_avx;

  char opt = 'N';
  int mesas = 0;
  int partidos = 0;
  puts("Introduzca el número de mesas electorales: ");
  scanf("%d", &mesas);
  puts("Introduzca número de partidos políticos: ");
  scanf("%d", &partidos);
  getchar();
  puts("Mostrar datos por pantalla (S/N): ");
  opt = getchar();

  /*Reserva de memoria en HEAP para procesar la mayor cantidad
  de numeros en una matriz*/

  float * m2 = NULL;
  if ((m2 = malloc(sizeof(float)*mesas*partidos)) == NULL)
  {
    perror("malloc error");
    return -1;
  }

  float * m3 = NULL;
  if ((m3 = malloc(sizeof(float)*mesas*partidos)) == NULL)
  {
    perror("malloc error");
    return -1;
  }

  float * total = NULL;
  if ((total = malloc(sizeof(float)*partidos)) == NULL)
  {
    perror("malloc error");
    return -1;
  }

  printf("Creando matriz con numeros aleatorios... \n");
  random_matrix_n(mesas, partidos, m2);
  printf("Matriz creada.\n");
  transpose(mesas, partidos, m2, m3);

  printf("Calculando total... \n");

  /* --C-- */

  gettimeofday(&iteInicio, NULL);
  ftotal(mesas, partidos, m3, total);   // T
  gettimeofday(&iteFinal, NULL);

  tiempo_c = ((double)iteFinal.tv_sec + (double)iteFinal.tv_usec/1000000) -
      ((double)iteInicio.tv_sec + (double)iteInicio.tv_usec/1000000);

  if (opt == 'S' || opt == 's')
    print_matrix2(mesas, partidos, m2, total);

  // Inicialización a 0, para comprobar que realmente se escribirá
  // en el siguiente método
  memset(total, 0, (sizeof(float)*partidos));
  printf("[C] Tiempo de cálculo para funcion total(): %f ''\n", tiempo_c);

  /* --AVX-- */

  gettimeofday(&iteInicio, NULL);
  total_avx(mesas, partidos, m3, total);  // T
  gettimeofday(&iteFinal, NULL);

  tiempo_avx = ((double)iteFinal.tv_sec + (double)iteFinal.tv_usec/1000000) -
      ((double)iteInicio.tv_sec + (double)iteInicio.tv_usec/1000000);


  if (opt == 'S' || opt == 's')
    print_matrix2(mesas, partidos, m2, total);

  printf("[AVX] Tiempo de cálculo para funcion total(): %f ''\n", tiempo_avx);

  free(m2);
  free(m3);
  free(total);
  //_mm_free(total);

  return 0;
}
