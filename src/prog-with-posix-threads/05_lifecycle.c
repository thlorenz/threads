/*
 * Demonstrates typical thread life cycle (Chapter 2 Threads: page 31)
 */

#include <pthread.h>
#include "errors.h"

void *thread_cb(void *arg) {
  pthread_t self = pthread_self();
  printf("running thread with id %d", (int)self);
  return arg;
}

int main(void) {
  pthread_t thread_id;
  void *thread_result;

  // pass NULL to thread_cb
  int status = pthread_create(&thread_id, NULL, thread_cb, NULL);
  if (status) err_abort(status, "create thread");

  // wait for thread to finish executing
  status = pthread_join(thread_id, &thread_result);
  if (status) err_abort(status, "join thread");

  // passed NULL as arg and returning that from thread_cb, so we expect return value to be NULL
  return thread_result == NULL ? 0 : 1;
}
