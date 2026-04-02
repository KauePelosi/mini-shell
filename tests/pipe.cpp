#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  int fd[2];
  pipe(fd);

  pid_t pid1 = fork();

  if (pid1 == 0) {
    dup2(fd[1], STDOUT_FILENO);
    close(fd[0]);
    close(fd[1]);

    execlp("echo", "echo", "hello", NULL);
  }

  pid_t pid2 = fork();

  if (pid2 == 0) {
    dup2(fd[0], STDIN_FILENO);
    close(fd[0]);
    close(fd[1]);

    execlp("wc", "wc", "-c", NULL);
  }

  close(fd[0]);
  close(fd[1]);
  wait(NULL);
  wait(NULL);

  return 0;
}
