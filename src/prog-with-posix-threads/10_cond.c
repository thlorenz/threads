/*
 * Demonstrates use of a convar in order to communicate changes to a piece of data
 * another thread is interested in.
 *
 * (Chapter 3.3.2 Waiting on a condition variable - page 77)
 */

#include <pthread.h>
#include <time.h>
#include "errors.h"

#define HIBERNATION 1

typedef struct data_s {
  pthread_mutex_t mutex;  // protects access to value
  pthread_cond_t cond;    // signals change to value
  int value;
} data_t;

data_t data = {
  PTHREAD_MUTEX_INITIALIZER,
  PTHREAD_COND_INITIALIZER,
  0
};

void *signal_thread(void *arg) {
  // hibernates, then sets the main thread's predicate and signals the condition
  while (1) {
    sleep(HIBERNATION);

    int status = pthread_mutex_lock(&data.mutex);
    if (status) err_abort(status, "lock mutex");

    data.value++;

    // signal wakes up ONE of the threads waiting on the convar
    // wake up more than one at a time vie pthread_cond_broadcast
    status = pthread_cond_signal(&data.cond);
    if (status) err_abort(status, "signal condition");

    // we could have unlocked the mutex BEFORE signaling which is more efficient
    // since the awakened thread succeeds immediately when locking the mutex
    // granted no other thread locked it in the meantime
    status = pthread_mutex_unlock(&data.mutex);
    if (status) err_abort(status, "unlock mutex");
  }

  return NULL;
}

int main(void) {
  pthread_t signal_thread_id;

  int status = pthread_create(&signal_thread_id, NULL, signal_thread, NULL);
  if (status) err_abort(status, "create signal thread");
  
  // wait for up to 2 seconds for signal_thread to signal the condition
  struct timespec timeout = {
    .tv_sec  = time(NULL) + 2,
    .tv_nsec = 0
  };

  status = pthread_mutex_lock(&data.mutex);
  if (status) err_abort(status, "lock mutex");

  while (data.value < 10) {
    // condition variable wait operation unlocks the mutex before blocking the thread
    // and relocks it before continuing
    status = pthread_cond_timedwait(&data.cond, &data.mutex, &timeout);
    if (status == ETIMEDOUT) {
      fprintf(stderr, "condition wait timed out.\n");
      break;
    }

    if (status) err_abort(status, "wait for condition");
    // NEVER assume that a predicate is true once we wake from a convar
    // In this case another thread could have locked the mutex first and changed the value
    if (data.value) printf("condition was signaled with value %d\n", data.value);
  }

  return 0;
}
