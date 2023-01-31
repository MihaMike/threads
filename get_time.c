#include <sys/time.h>
#include <sys/resource.h>
#include "get_time.h"

/* Вернуть процессорное время, затраченное на текущий
   поток, в сотых долях секунды. Берется время только
   самого потока, время работы системных вызовов не
   прибавляется. */
long int get_time (void)
{
  struct rusage buf;

  getrusage (RUSAGE_THREAD, &buf);
           /* преобразуем время в секундах
              в сотые доли секунды */
  return   buf.ru_utime.tv_sec * 100
           /* преобразуем время в микросекундах 
              в сотые доли секунды */
         + buf.ru_utime.tv_usec / 10000;
}

/* Возвращает астрономическое (!) время 
   в сотых долях секунды */
long int get_full_time (void)
{
  struct timeval buf;

  gettimeofday (&buf, 0);
           /* преобразуем время в секундах
              в сотые доли секунды */
  return   buf.tv_sec * 100
           /* преобразуем время в микросекундах 
              в сотые доли секунды */
         + buf.tv_usec / 10000;
}
