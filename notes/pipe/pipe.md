# Pipe

## What is

Pipe is a mechanism to connect one
command output (STDOUT_FILENO) to another
command like an input (STDIN_FILENO).

Represented by the symbol `|` pipe is frequently used in terminals or Linux environment.

### Exemple

#### Process map

Process → write → STDOUT → pipe → STDIN → next process

#### Command

```bash
ls -l | grep ".cpp"
```

## How it works

Above we have the commands `ls -l` which prints the files
in a specific folder and the command `grep ".cpp"`
which prints only the files with the extension .cpp

This is just possible because pipe connect
the `ls -l` output to the `grep ".cpp"` input.

### In memory

In memory, pipe is created by kernel,
which manage its size and communicate
using File Descriptors - `fd`.

In Linux, everything is a file, that's the `fd` importance,
using them kernel say what is happen
with the process and talk this to pipe.

#### SysCall (System Calls)

Taking a closer look at pipe, we have the syscalls, its the interface
between a user program and the kernel.
It is used when your program need something just kernel can do,
like creating a process or a communication channel.

##### SysCall Exemples

1. Fork()
   Fork() is the SysCall responsible for providing a way
   to create a new process.

   **Example:**

   ```cpp
   pid_t pid = fork();
   ```

   **Explanation**
   `pid_t` Its an specific type used to represented the Process Id (Pid).

2. Exec()
   Exec() is a family of SysCalls responsible for replacing the
   current process image with a new program.

   **Exemple**

   ```cpp
   execvp("ls", args);
   ```

   **Explanation**
   `execvp` searches for the program in PATH and passes `args`
   (a null-terminated array) as argument.

3. Pipe()
   Pipe() is a SysCall responsible to created an unidirectional data channel
   between two File Descriptors.

   **Example**

   ```cpp
   int fd[2];
   pipe(fd);
   ```

   **Explanation**
   `fd[0]` holds the read end and `fd[1]` holds the write end.
   Data written with `fd[1]` can be read from `fd[0]`.

4. Close()
   Close() is the SysCall responsible for releasing a file descriptor,
   signaling EOF to the other end of a pipe.
   **Example:**

```cpp
   close(fd[0]);
   close(fd[1]);
```

**Explanation**
Always close unused ends in both parent and child. Leaving a write
end open prevents `read()` from ever returning EOF.

1. Dup2()
   Dup2() is the SysCall responsible for duplicating a file descriptor
   into a specific target, commonly used to redirect stdin/stdout.
   **Example:**

```cpp
   dup2(fd[1], STDOUT_FILENO);
```

**Explanation**
The target fd (`STDOUT_FILENO`) is closed and replaced by a copy
of the source fd (`fd[1]`). Any write to stdout will now go
into the pipe.
