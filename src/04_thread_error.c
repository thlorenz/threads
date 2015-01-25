#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(void) {
  pthread_t thread;
  
  // joining uninitialized thread ID will return ESRCH error code in most cases
  int status = pthread_join(thread, NULL);
  if (status) fprintf(stderr, "error %d: %s\n", status, strerror(status));
  return status;
}
