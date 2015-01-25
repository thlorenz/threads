#include <sys/types.h>

#include "errors.h"

void schedule_alarm(int seconds, char* message) {
  // child_process
  sleep(seconds);
  printf("(%d) %s\n", seconds, message);
  exit(0);
}

void collect_terminated_children() {
  // parent process
  int pid;
  
  // collect any terminated children via waitpid 
  do {
    pid = waitpid((pid_t)-1, NULL, WNOHANG);    
    if (pid == (pid_t)-1) errno_abort("waitpid for child");
  } while (pid != (pid_t)0);

}

int main(void) {
  char line[128];
  char message[64];
  int status;
  int seconds;
  pid_t pid;

  while (1) {
    printf("alarm > ");
    if (fgets(line, sizeof (line), stdin) == NULL) exit(0);
    if (strlen(line) <= 1) continue;

    if (sscanf(line, "%d %64[^\n]", &seconds, message) < 2) {
      fprintf(stderr, "Bad command\n");
    } else {
      pid = fork();
      if (pid == (pid_t)-1) errno_abort("fork");

      if (pid == (pid_t)0) schedule_alarm(seconds, message);
      else collect_terminated_children();
    }
  }

  return 0;
}
