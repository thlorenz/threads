/*
 * Demonstrates how to access a var (time_t time) from 2 threads
 * and synchronize via a mutex non-blocking.
 * This is useful to allow your thread to do useful work while
 * waiting on a mutex.
 *
 * (Chapter 3.2.2.1 Nonblocking mutex locks - page 58)
 */

#include <pthread.h>
#include "errors.h"

#define SPIN 100000000

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
long counter;
time_t end_time;

void *counter_handler(void *arg) {
  // repeatedly locks a mutex and increments a counter multiple times
  // while doing that the mutex is locked to other threads
  while (time(NULL) < end_time) {

    int status = pthread_mutex_lock(&mutex);
    if (status) err_abort(status, "lock mutex");

    for (int spin = 0; spin < SPIN; spin++) counter++;

    status = pthread_mutex_unlock(&mutex);
    if (status) err_abort(status, "unlock mutex");

    sleep(1);
  }
  return NULL;
}

void *monitor_handler(void *arg) {
  // monitors the counter
  // tries to lock the mutex every 3 seconds, skips cycle if lock fails

  int misses = 0;

  while (time(NULL) < end_time) {
    sleep(3);
    int status = pthread_mutex_trylock(&mutex);

    if (status == EBUSY) {
      misses++;
      continue;
    }

    if (status) err_abort(status, "trylock mutex");
    printf("counter is %ld\n", counter/SPIN);

    status = pthread_mutex_unlock(&mutex);
    if (status) err_abort(status, "unlock mutex");
  }
  printf("monitor thread missed update %d times.\n", misses);
  return NULL;
}

int main(void) {
  pthread_t counter_thread_id;
  pthread_t monitor_thread_id;

  end_time = time(NULL) + 60;
  int status = pthread_create(&counter_thread_id, NULL, counter_handler, NULL);
  if (status) err_abort(status, "create counter thread");
  
  status = pthread_create(&monitor_thread_id, NULL, monitor_handler, NULL);
  if (status) err_abort(status, "create monitor thread");

  status = pthread_join(counter_thread_id, NULL);
  if (status) err_abort(status, "join counter thread");

  status = pthread_join(monitor_thread_id, NULL);
  if (status) err_abort(status, "join monitor thread");

  return 0;
}
