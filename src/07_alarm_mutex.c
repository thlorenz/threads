/*
 * Demonstrates how to access a var (time_t time) from 2 threads
 * and synchronize via a mutex.
 *
 * (Chapter 3.2.2 Locking and unlocking a mutex - page 32)
 */

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
alarm_t *alarm_list = NULL;

void *alarm_handler(void *arg) {
  // processes all queued alarms
  int sleep_time;
  while (1) {
    int status = pthread_mutex_lock(&alarm_mutex);
    if (status) err_abort(status, "lock mutex");

    alarm_t *alarm = alarm_list;

    // if alarm list is empty wait for one second to allow main thread to read another command
    if (alarm == NULL) { 
      sleep_time = 1;
    } else {
      // if alarm is expired fire it (sched_yield), otherwise sleep for remaining time
      alarm_list = alarm->link; // unshift alarm from alarm_list
      time_t now = time(NULL);
      sleep_time = alarm->time <= now ? 0 : alarm->time - now;
#ifdef DEBUG
      fprintf(stderr, "[ waiting: %ld (%d) \"%s\" ]\n", alarm->time, sleep_time, alarm->message);
#endif
    }

    // unlock mutex before waiting to allow main thread to insert more alarms
    status = pthread_mutex_unlock(&alarm_mutex);
    if (status) err_abort(status, "unlock mutex");

    // sched_yield causes the calling thread to relinquish the CPU
    // @see: http://man7.org/linux/man-pages/man2/sched_yield.2.html
    if (sleep_time > 0) sleep(sleep_time); else sched_yield();

    // alarm expired
    if (alarm != NULL) {
      printf("[ expired: (%d) %s ]\n", alarm->seconds, alarm->message);
      free(alarm);
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
}
