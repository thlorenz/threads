/*
 * Demonstrates how to avoid mutex deadlocks by applying a backoff mechanism.
 *
 * (Chapter 3.2.5.1 Lock hierarchy - page 63)
 */

#include <pthread.h>
#include "errors.h"

#define ITER 10
#define BACKOFF 1
#define YIELD_FLAG 0 // 0: no yield, >0: yield, <0: sleep

pthread_mutex_t mutexes[3] = {
  PTHREAD_MUTEX_INITIALIZER,
  PTHREAD_MUTEX_INITIALIZER,
  PTHREAD_MUTEX_INITIALIZER
};

void maybe_yield() {
  if (!YIELD_FLAG) return;
  if (YIELD_FLAG > 0) sched_yield(); else sleep(1);
}

void *lock_forward(void *arg) {
  // locks all mutexes in order and thus conflicts with lock_backward
  int status;

  for (int iterate = 0; iterate < ITER; iterate++) {
    int backoffs = 0;

    for (int i = 0; i < 3; i++) {
      if (i == 0) {
        status = pthread_mutex_lock(&mutexes[i]);
        if (status) err_abort(status, "first lock");
      } else {
        status = BACKOFF ? pthread_mutex_trylock(&mutexes[i]) : pthread_mutex_lock(&mutexes[i]);
        if (status == EBUSY) {
          backoffs++;
          DPRINTF ((" [ forward locker backing off at %d ]\n", i));
          for (i--; i >= 0; i--) {
            status = pthread_mutex_unlock(&mutexes[i]);
            if (status) err_abort(status, "backoff unlock");
          }
        } else {
          if (status) err_abort(status, "lock mutex");
          DPRINTF ((" [ forward locker got %d ]\n", i));
        }
      }
      maybe_yield(); 
    }

    printf("lock forward got all locks, %d backoffs\n", backoffs);
    // unlocking in reverse order reduces the chance that another thread will need to back off.
    for (int i = 2; i >= 0; i--) {
      status = pthread_mutex_unlock(&mutexes[i]);
      if (status) err_abort(status, "unlock mutex");
    }
  }
  return NULL;
}

void *lock_backward(void *arg) {
  // locks all mutexes in reverse order and thus conflicts with lock_forward
  int status;

  for (int iterate = 0; iterate < ITER; iterate++) {
    int backoffs = 0;

    for (int i = 2; i >= 0; i--) {
      if (i == 0) {
        status = pthread_mutex_lock(&mutexes[i]);
        if (status) err_abort(status, "first lock");
      } else {
        status = BACKOFF ? pthread_mutex_trylock(&mutexes[i]) : pthread_mutex_lock(&mutexes[i]);
        if (status == EBUSY) {
          backoffs++;
          DPRINTF ((" [ backward locker backing off at %d ]\n", i));
          for (i++; i < 3; i++) {
            status = pthread_mutex_unlock(&mutexes[i]);
            if (status) err_abort(status, "backoff unlock");
          }
        } else {
          if (status) err_abort(status, "lock mutex");
          DPRINTF ((" [ backward locker got %d ]\n", i));
        }
      }
      maybe_yield(); 
    }

    printf("lock backward got all locks, %d backoffs\n", backoffs);
    for (int i = 0; i < 3; i++) {
      status = pthread_mutex_unlock(&mutexes[i]);
      if (status) err_abort(status, "unlock mutex");
    }
  }
  return NULL;
}

int main(void) {
  pthread_t forward_thread_id, backward_thread_id;

  int status = pthread_create(&forward_thread_id, NULL, lock_forward, NULL);
  if (status) err_abort(status, "create forward thread");

  status = pthread_create(&backward_thread_id, NULL, lock_backward, NULL);
  if (status) err_abort(status, "create backward thread");
  
  pthread_exit(NULL);
  return 0;
}
