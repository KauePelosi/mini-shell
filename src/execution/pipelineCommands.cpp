#include "execution/pipelineCommands.hpp"
#include "config.hpp"
#include <array>
#include <cerrno>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <ostream>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <signal.h>

int pipelineCommands(
    const std::vector<std::vector<std::string>> &pipeCommands) {

  const size_t commandCount = pipeCommands.size();
  size_t pipeNumber = commandCount - 1;

  std::vector<std::array<int, 2>> pipes(pipeNumber);

  for (size_t i = 0; i < pipeNumber; i++) {
    if (pipe(pipes[i].data()) == -1) {
      std::cerr << shellName << ": failed to create pipe - "
                << std::strerror(errno) << std::endl;
      _exit(1);
    }
  }

  std::vector<pid_t> pids(commandCount);

  for (size_t i = 0; i < commandCount; i++) {
    pids[i] = fork();

    if (pids[i] == -1) {
      std::cerr << shellName << ": failed to create pids - "
                << std::strerror(errno) << std::endl;
      _exit(1);
    }

    if (pids[i] == 0) {
      signal(SIGINT, SIG_DFL);
      if (i > 0) {
        if (dup2(pipes[i - 1][0], STDIN_FILENO) == -1) {
          std::cerr << shellName << ": failed to redirect STDIN - "
                    << std::strerror(errno) << std::endl;
          _exit(1);
        }
      }

      if (i < pipeNumber) {
        if (dup2(pipes[i][1], STDOUT_FILENO) == -1) {
          std::cerr << shellName << ": failed to redirect STDOUT - "
                    << std::strerror(errno) << std::endl;
          _exit(1);
        }
      }

      for (const auto &fds : pipes) {
        close(fds[0]);
        close(fds[1]);
      }

      std::vector<char *> args;

      for (const auto &arg : pipeCommands[i]) {
        args.push_back(const_cast<char *>(arg.c_str()));
      }
      args.push_back(nullptr);

      if (execvp(args[0], args.data()) == -1) {
        std::cerr << shellName << ": command not found - " << args[0];
        _exit(127);
      }
    }
  }

  for (const auto &fds : pipes) {
    close(fds[0]);
    close(fds[1]);
  }

  int status = 0;

  for (pid_t pid : pids) {
    waitpid(pid, &status, 0);
  }

  return WEXITSTATUS(status);
}
