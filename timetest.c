/* timetest.c - program to test time measurement functions */
/* Use clock_gettime in linux, clock_get_time in OS X. */

/* Allow compilation under c99 and POSIX 1995 / 2004. */
#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif /* __STDC_VERSION__ */

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

/* See http://wiki.geeky-boy.com/w/index.php?title=Internal_error_handling */
#define WCHK(cond_expr) do { \
  if (!(cond_expr)) { \
    fprintf(stderr, "%s:%d, Warning, expected '%s' to be true (%s)\n", \
      __FILE__, __LINE__, #cond_expr, strerror(errno)); \
  }  \
} while (0)

#define ECHK(cond_expr) do { \
  if (!(cond_expr)) { \
    fprintf(stderr, "%s:%d, Error, expected '%s' to be true (%s)\n", \
      __FILE__, __LINE__, #cond_expr, strerror(errno)); \
    abort(); \
  }  \
} while (0)


/* global */
#ifdef __MACH__
clock_serv_t cclock;
#endif


#ifdef __MACH__
int mono_time_get(struct timespec *ts){
  mach_timespec_t mts;
  if (clock_get_time(cclock, &mts) != KERN_SUCCESS)
    return -1;
  ts->tv_sec = mts.tv_sec;
  ts->tv_nsec = mts.tv_nsec;
  return 0;
}  /* mono_time_get */
#else
#define mono_time_get(ts) clock_gettime(CLOCK_MONOTONIC, ts);
#endif

int mono_time_init()
{
#ifdef __MACH__
  if (host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock) != KERN_SUCCESS)
    return -1;
#endif

  struct timespec ts;
  return mono_time_get(&ts);
}  /* mono_time_init */

int mono_time_cleanup()
{
#ifdef __MACH__
  if (mach_port_deallocate(mach_task_self(), cclock) != KERN_SUCCESS)
    return -1;
  else
    return 0;
#else
  return 0;
#endif
}  /* mono_time_cleanup */


int main(int argc, char **argv)
{
  mono_time_init();
  sleep(1);

#ifndef __MACH__
  struct timespec ts;
  int st = clock_getres(CLOCK_MONOTONIC, &ts); ECHK(st == 0);
#endif

  struct timespec first1[3], last1[3], first2[3], last2[3], first3[3], last3[3];

  for (int j = 0; j < 3; j++) {
    mono_time_get(&first1[j]);
    for (int i = 0; i < 1000000; i++) {
      mono_time_get(&last1[j]);
    }

    mono_time_get(&first2[j]);
    mono_time_get(&last2[j]);
    mono_time_get(&last2[j]);
    mono_time_get(&last2[j]);
    mono_time_get(&last2[j]);
    mono_time_get(&last2[j]);
    mono_time_get(&last2[j]);
    mono_time_get(&last2[j]);
    mono_time_get(&last2[j]);
    mono_time_get(&last2[j]);
    mono_time_get(&last2[j]);

    mono_time_get(&first3[j]);
    mono_time_get(&last3[j]);
  }

  printf("timetest.c\n");
#ifndef __MACH__
  printf("clock_getres: %ld %ld\n", (long)ts.tv_sec, (long)ts.tv_nsec);
#endif

  for (int j = 0; j < 3; j++) {
    double delta1 = (last1[j].tv_sec - first1[j].tv_sec) + (last1[j].tv_nsec - first1[j].tv_nsec) * 1e-9;
    printf("delta1=%e s\n", delta1/1000000);
    double delta2 = (last2[j].tv_sec - first2[j].tv_sec) + (last2[j].tv_nsec - first2[j].tv_nsec) * 1e-9;
    printf("delta2=%e s\n", delta2/10);
    double delta3 = (last3[j].tv_sec - first3[j].tv_sec) + (last3[j].tv_nsec - first3[j].tv_nsec) * 1e-9;
    printf("delta3=%e s\n", delta3);
  }

  mono_time_cleanup();
  return 0;
}  /* main */
