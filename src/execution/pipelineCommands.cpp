#include "execution/pipelineCommands.hpp"
#include "config.hpp"
#include <array>
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

int pipelineCommands(
    const std::vector<std::vector<std::string>> &pipeCommands) {

  const size_t commandCount = pipeCommands.size();
  size_t pipeNumber = commandCount - 1;

  std::vector<std::array<int, 2>> pipes(pipeNumber);
  for (size_t i = 0; i < pipeNumber; i++) {
    if (pipe(pipes[i].data()) == -1) {
      std::cerr << shellName << ": failed to create pipe - "
                << std::strerror(errno) << std::endl;
      return -1;
    }
  }
  std::vector<pid_t> pids(commandCount);
  for (size_t i = 0; i < commandCount; i++) {
    pids[i] = fork();

    if (pids[i] == -1) {
      std::cerr << shellName << ": failed to create pids - "
                << std::strerror(errno) << std::endl;
      return -1;
    }

    if (pids[i] == 0) {
      if (i > 0) {
        if (dup2(pipes[i - 1][0], STDIN_FILENO) == -1) {
        }
      }
    }
  }

  return 0;
}
