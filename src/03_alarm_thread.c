#include <pthread.h>

#include "errors.h"

typedef struct alarm_s {
  int   seconds;
  char  message[64];
} alarm_t;

void *alarm_thread(void *arg) {
  // arg is 4th arg passed to pthread_create
  alarm_t *alarm = arg;

  // detach in order to signal that app doesn't need to know if/how thread terminates
  int status = pthread_detach(pthread_self());
  if (status) err_abort(status, "detach thread");

  sleep(alarm->seconds);
  printf("(%d) %s\n", alarm->seconds, alarm->message);

  free(alarm);
  return NULL;
}

int main(void) {
  char line[128];
  pthread_t thread;

  while (1) {
    printf("alarm > ");
    if (fgets(line, sizeof (line), stdin) == NULL) exit(0);
    if (strlen(line) <= 1) continue;
  
    alarm_t *alarm = malloc(sizeof(alarm_t));
    if (alarm == NULL) errno_abort("allocate alarm");

    if (sscanf(line, "%d %64[^\n]", &alarm->seconds, alarm->message) < 2) {
      fprintf(stderr, "Bad command\n");
      free(alarm);
    } else {
      int status = pthread_create(&thread, NULL, alarm_thread, alarm);
      if (status) err_abort(status, "create alarm thread");
    }
  }
  return 0;
}
