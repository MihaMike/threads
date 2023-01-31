#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "matrices.h"
#include "get_time.h"

/* Аргументы для потока */
typedef struct _ARGS
{
  double *matrix;       /* матрица */
  double *vector;       /* вектор */
  double *result;       /* результирующий вектор */
  int n;                /* размер матрицы и векторов */
  int thread_num;       /* номер задачи */
  int total_threads;    /* всего задач */
} ARGS;

/* Суммарное время работы всех задач */
static long int threads_total_time = 0;
/* Объект типа mutex для синхронизации доступа к
   threads_total_time */
static pthread_mutex_t threads_total_time_mutex 
  = PTHREAD_MUTEX_INITIALIZER;

/* Количество тестов (для отладки) */
#define N_TESTS 10

/* Умножение матрицы на вектор для одной задачи */
static void * matrix_mult_vector_threaded (void *pa)
{
  ARGS *pargs = (ARGS*)pa;
  long int t;
  int i;

  printf ("Thread %d started\n", pargs->thread_num);
  t = get_time ();      /* время начала работы */
  for (i = 0; i < N_TESTS; i++)
  {
    matrix_mult_vector (pargs->matrix, pargs->vector,
                        pargs->result, pargs->n,
                        pargs->thread_num,
                        pargs->total_threads);
    printf ("Thread %d mult %d times\n",
            pargs->thread_num, i);
  }
  t = get_time () - t;  /* время конца работы */

  /* Суммируем времена работы */
  /* "захватить" mutex для работы с threads_total_time */
  pthread_mutex_lock (&threads_total_time_mutex);
  threads_total_time += t;
  /* "освободить" mutex */
  pthread_mutex_unlock (&threads_total_time_mutex);
  printf ("Thread %d finished, time = %ld\n",
          pargs->thread_num, t);

  return 0;
}


int main (int argc, char * argv[])
{
  /* массив идентификаторов созданных задач */
  pthread_t * threads;
  /* массив аргументов для созданных задач */
  ARGS * args;
  /* число создаваемых задач */
  int nthreads;
  /* астрономическое время работы всего процесса */
  long int t_full;

  int n;                /* размер матрицы и векторов */
  double *matrix;       /* матрица */
  double *vector;       /* вектор */
  double *result;       /* результирующий вектор */
  int i, l;

  if (argc != 3
      || !(nthreads = atoi (argv[1]))
      || !(n = atoi (argv[2])))
    {
      printf ("Usage: %s nthreads n\n", argv[0]);
      return 1;
    }

  if (!(threads = (pthread_t*)
                  malloc (nthreads * sizeof (pthread_t))))
    {
      fprintf (stderr, "Not enough memory!\n");
      return 1;
    }
  if (!(args = (ARGS*) malloc (nthreads * sizeof (ARGS))))
    {
      fprintf (stderr, "Not enough memory!\n");
      return 2;
    }

  /* Выделение памяти под массивы */
  if (!(matrix = (double*)
                 malloc (n * n * sizeof (double))))
    {
      fprintf (stderr, "Not enough memory!\n");
      return 3;
    }
  if (!(vector = (double*) malloc (n * sizeof (double))))
    {
      fprintf (stderr, "Not enough memory!\n");
      return 4;
    }
  if (!(result = (double*) malloc (n * sizeof (double))))
    {
      fprintf (stderr, "Not enough memory!\n");
      return 5;
    }

  /* Инициализация массивов */
  init_matrix (matrix, n);
  init_vector (vector, n);
  printf ("Matrix:\n");
  print_matrix (matrix, n);
  printf ("Vector:\n");
  print_vector (vector, n);

  l = (n * n + 2 * n) * sizeof (double);
  printf ("Allocated %d bytes (%dKb or %dMb) of memory\n",
          l, l >> 10, l >> 20);

  /* Инициализация аргументов задач */
  for (i = 0; i < nthreads; i++)
    {
      args[i].matrix = matrix;
      args[i].vector = vector;
      args[i].result = result;
      args[i].n = n;
      args[i].thread_num = i;
      args[i].total_threads = nthreads;
    }

  /* Засекаем астрономическое время начала работы задач*/
  t_full = get_full_time ();
  /* Запускаем задачи */
  for (i = 0; i < nthreads; i++)
    {
      if (pthread_create (threads + i, 0,
                          matrix_mult_vector_threaded,
                          args + i))
        {
          fprintf (stderr, "cannot create thread #%d!\n",
                   i);
          return 10;
        }
    }
  /* Ожидаем окончания задач */
  for (i = 0; i < nthreads; i++)
    {
      if (pthread_join (threads[i], 0))
        fprintf (stderr, "cannot wait thread #%d!\n", i);
    }
  t_full = get_full_time () - t_full;
  if (t_full == 0)
    t_full = 1; /* очень быстрый компьютер... */

  /* Здесь можно работать с результатом */
  print_vector (result, n);

  /* Освобождаем память */
  free (threads);
  free (args);
  free (matrix);
  free (vector);
  free (result);

  printf ("Total full time = %ld, \
total threads time = %ld (%ld%%), per thread = %ld\n",
          t_full, threads_total_time,
          (threads_total_time * 100) / t_full,
          threads_total_time / nthreads);

  return 0;
}
