#include <pthread.h>
#include <time.h>

#include "errors.h"

typedef struct alarm_s {
  struct alarm_s *link;
  int seconds;
  time_t time;
  char message[64];
} alarm_t;

pthread_mutex_t alarm_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t alarm_cond = PTHREAD_COND_INITIALIZER;
alarm_t *alarm_list = NULL;
time_t current_alarm = 0;

void *alarm_handler(void *arg) {
  // waits on signals to process queued alarms

  // locked mutex will be unlocked during condition waits, so we lock it in the beginning
  int status = pthread_mutex_lock(&alarm_mutex);
  if (status) err_abort(status, "lock mutex");

  while (1) {
    // let main thread know that the handler is not currently busy processing an alarm
    current_alarm = 0;

    // wait for alarm to get queued
    while (alarm_list == NULL) {
      status = pthread_cond_wait(&alarm_cond, &alarm_mutex);
      if (status) err_abort(status, "wait on cond");
    }

    // unshift alarm from list and handle it
    alarm_t *alarm = alarm_list;
    alarm_list = alarm->link;
    time_t now = time(NULL);

    int expired = 0;
    if (alarm->time > now) {
      // current alarm time is in the future
#ifdef DEBUG
      fprintf(stderr, "[ waiting: %ld (%ld) \"%s\" ]\n", alarm->time, alarm->time - now, alarm->message);
#endif

      struct timespec timeout = {
        .tv_sec  = alarm->time,
        .tv_nsec = 0
      };
      current_alarm = alarm->time;

      while(current_alarm == alarm->time) {
        // wait to get a signal for an alarm that will expire sooner than the current one
        // or that our current alarm expires (timeout)
        status = pthread_cond_timedwait(&alarm_cond, &alarm_mutex, &timeout);
        if (status == ETIMEDOUT) {
          expired = 1;
          break;
          // continue in the loop to pick up the next alarm to expire
        }
        if (status) err_abort(status, "cond timedwait");

        // continue in the loop to pick up the alarm that expires earlier than the one
        // we were handling
      }

    } else {
      // current alarm time had passed
      expired = 1;
    }

    if (expired) {
      printf ("(%d) %s\n", alarm->seconds, alarm->message);
      free (alarm);
    }
  }
}

void print_alarms() {
  time_t now = time(NULL);

  printf("[ list of alarms following the one we are waiting on: ");

  for (alarm_t *next = alarm_list; next != NULL; next = next->link) 
    printf ("\n\t%ld (%ld) \"%s\", ", next->time, next->time - now, next->message);

  printf ("\n]\n");
}

void add_alarm(alarm_t *alarm) {
  // add alarm to alarms into right position to maintain expiration date sorting 
  // caller needs to have locked alarm_mutex since we are accessing alarm_list
  alarm_t **last = &alarm_list;
  alarm_t *next = *last;
  while (next != NULL) {
    if (next->time >= alarm->time) {
      alarm->link = next;
      *last = alarm;
      break;
    }
    last = &next->link;
    next = next->link;
  }

  if (next == NULL) {
    *last = alarm;
    alarm->link = NULL;
  }

#ifdef DEBUG
  print_alarms();
#endif

  // current_alarm == 0 signals that alarm handler is waiting for work
  // otherwise new alarm could come before the alarm it's handling
  // -> in either case wake it up
  if (current_alarm == 0 || alarm->time < current_alarm) {
    current_alarm = alarm->time;
    int status = pthread_cond_signal(&alarm_cond);
    if (status) err_abort(status, "signal cond");
  }
}

int main(void) {
  char line[128];
  pthread_t thread;

  int status = pthread_create(&thread, NULL, alarm_handler, NULL);
  if (status) err_abort(status, "create thread");

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
      int status = pthread_mutex_lock(&alarm_mutex);
      time_t now = time(NULL);
      if (status) err_abort(status, "lock mutex");
      alarm->time = now + alarm->seconds;
      add_alarm(alarm);
#ifdef DEBUG
      print_alarms();
#endif
      status = pthread_mutex_unlock(&alarm_mutex);
      if (status) err_abort(status, "lock mutex");
    }
  }
  return 0;
}
