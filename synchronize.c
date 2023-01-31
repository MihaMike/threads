#include <pthread.h>
#include "synchronize.h"

/* Дождаться в текущем потоке остальных потоков
   (из общего числа total_threads). */
void synchronize (int total_threads)
{
  /* Объект синхронизации типа mutex */
  static pthread_mutex_t mutex
    = PTHREAD_MUTEX_INITIALIZER;
  /* Объект синхронизации типа condvar */
  static pthread_cond_t condvar_in
    = PTHREAD_COND_INITIALIZER;
  /* Объект синхронизации типа condvar */
  static pthread_cond_t condvar_out
    = PTHREAD_COND_INITIALIZER;
  /* Число пришедших в функцию задач */
  static int threads_in = 0;
  /* Число ожидающих выхода из функции задач */
  static int threads_out = 0;

  /* "захватить" mutex для работы с переменными 
     threads_in и threads_out */
  pthread_mutex_lock (&mutex);

  /* увеличить на 1 количество прибывших в
     эту функцию задач */
  threads_in++;

  /* проверяем количество прибывших задач */
  if (threads_in >= total_threads)
    {
      /* текущий поток пришел последним */
      /* устанавливаем начальное значение
         для threads_out */
      threads_out = 0;

      /* разрешаем остальным продолжать работу */
      pthread_cond_broadcast (&condvar_in);
    }
  else
    {
      /* есть еще не пришедшие потоки */

      /* ожидаем, пока в эту функцию не придут
         все потоки */
      while (threads_in < total_threads)
        {
          /* ожидаем разрешения продолжить работу:
             освободить mutex и ждать сигнала от
             condvar, затем "захватить" mutex опять */
          pthread_cond_wait (&condvar_in, &mutex);
        }
    }

  /* увеличить на 1 количество ожидающих выхода задач */
  threads_out++;

  /* проверяем количество прибывших задач */
  if (threads_out >= total_threads)
    {
      /* текущий поток пришел в очередь последним */
      /* устанавливаем начальное значение
         для threads_in */
      threads_in = 0;

      /* разрешаем остальным продолжать работу */
      pthread_cond_broadcast (&condvar_out);
    }
  else
    {
      /* в очереди ожидания еще есть потоки */

      /* ожидаем, пока в очередь ожидания не придет
         последний поток */
      while (threads_out < total_threads)
        {
          /* ожидаем разрешения продолжить работу:
             освободить mutex и ждать сигнала от
             condvar, затем "захватить" mutex опять */
          pthread_cond_wait (&condvar_out, &mutex);
        }
    }

  /* "освободить" mutex */
  pthread_mutex_unlock (&mutex);
}
