/*
 * Dynamic mutex initialization: (Chapter 3.2.1: Creating and destroying a mutex - page 49)
 */

#include <pthread.h>
#include "errors.h"

typedef struct data_s {
  pthread_mutex_t mutex; // protects value
  int             value;
} data_t;

int main(void) {
  data_t *data = malloc(sizeof(data_t));
  if (data == NULL) errno_abort("allocate structure");

  int status = pthread_mutex_init(&data->mutex, NULL);
  if (status) err_abort(status, "init mutex");

  // may destroy mutex as soon as we are sure that no threads are blocked on the mutex
  // destroy it before freeing storage it occupies
  status = pthread_mutex_destroy(&data->mutex);
  if (status) err_abort(status, "destroy mutex");

  free(data);
  return status;
}
