/*
 * Demonstrates how to send SIGPROF to threads from main thread via pthread_kill().
 * These signals are handled inside `on_sigprof` on the non-main threads.
 * When a signal is caught the backtrace of that thread is printed.
 *
 * This solution is different from the alternative which installs a ITIMER_PROF timer that 
 * will send SIGPROF signals automatically. These signals are caught on the main thread.
 *
 * Sample Code of alternative:
 *
   static struct itimerval timer;

   timer.it_interval.tv_sec = 0;
   timer.it_interval.tv_usec = 1000000 / 100; // 100hz
   timer.it_value = timer.it_interval;

   setitimer(ITIMER_PROF, &timer, NULL);
 
*/ 

#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include <assert.h>

#include "stacktrace.h"

const int thread_count = 4;
volatile sig_atomic_t signal_count[thread_count];
typedef struct thread_s { 
  pthread_t pthread;
  int id;
} thread_t;

pthread_mutex_t stacktrace_mutex = PTHREAD_MUTEX_INITIALIZER;
thread_t threads[thread_count];

static void on_sigprof(int signal, siginfo_t* info, void* context) {
  if (signal != SIGPROF) return;

  pthread_t pthread = pthread_self();

  int id = -1;
  for (int t = 0; t < thread_count; t++)
    if (threads[t].pthread == pthread) id = threads[t].id;

  // ensure two threads don't dump the stack at the same time
  int status = pthread_mutex_lock(&stacktrace_mutex);
  assert(!status && "lock mutex");

  time_t now = time(NULL);
  fprintf(stderr, "Caught SIGPROF at %ld on thread: %d\n", now, id);
  dump_stacktrace();

  status = pthread_mutex_unlock(&stacktrace_mutex);
  assert(!status && "unlock mutex");
}

static void install_sig_handler() {
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_sigaction = on_sigprof; 
  sigemptyset(&sa.sa_mask);

  sigaction(SIGPROF, &sa, NULL);
}

static void sample_time(int seconds) {
  time_t now = time(NULL);
  int count = 0;
  while (1) {
    if (time(NULL) >= now + seconds) break;
    if (++count > 1E6) {
      count = 0;
      for (int t = 0; t < thread_count; t++) {
        pthread_kill(threads[t].pthread, SIGPROF);
      }
    }
  }
}

static void bar(int seconds) {
  time_t now = time(NULL);
  while (1) {
    if (time(NULL) >= now + seconds) break;
  }
}

static void foo(int seconds) {
  time_t now = time(NULL);
  bar(seconds / 2);
  while (1) {
    if (time(NULL) >= now + seconds) break;
  }
}

static void idle_time(int seconds) {
  time_t now = time(NULL);
  while (1) {
    foo(seconds / 2);
    if (time(NULL) >= now + seconds) break;
  }
}

static void* idler(void* args) {
  install_sig_handler();
  idle_time(5);
  return NULL;
}



int main(void) {
  for (int t = 0; t < thread_count; t++) {
    threads[t].id = t;
    int status = pthread_create(&threads[t].pthread, NULL, idler, NULL);
    assert(!status && "pthread create");
    signal_count[t] = 0;
  }
  sample_time(5);

  return 0;
}
