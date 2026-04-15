# Pipeline Execution

## Objective

This note explains how pipeline execution works,
detail the technical decisions and implementation logic
behind process management and inter-process communication (IPC).

## Pipe

The first step to understanding pipelines start with pipes.
Pipes it's a mechanism to communicate between the command input and output
besides how the operating system works with pipe.

### 1. Building a pipe

On C++ and Unix based systems we need to import the libraries
**unistd.h** and **sys/wait.h** to use functions like:

- pipe();
- fork();
- execvp();
- dup2();
- close();

Below is a simple example to pipe implementation:

```cpp
#include <unistd.h>
#include <sys/wait.h>

int main() {
  int fd[2];
  pipe(fd);

  if (fork() == 0) {
    dup2(fd[1], STDOUT_FILENO);
    close(fd[0]);
    execpl("ls", "ls", NULL);

  } else {
    dup2(fd[0], STDIN_FILENO);
    close(fd[1]);
    execlp("grep", "grep", "cpp", NULL);

  }
  return 0;
}
```

- `fd[2]` represents the two ends of a pipe:
  - `fd[0]` is the read end (STDIN_FILENO);
  - `fd[1]` is the write end (STDOUT_FILENO);

- `pipe(fd)` creates the pipe, which works similarly
  to a physical pipe.

- `fork() == 0` creates one new process that is
  a copy of the calling process.

- `dup2()` redirects a file descriptor to another,
  allowing the output of one process to become
  the input of another.
