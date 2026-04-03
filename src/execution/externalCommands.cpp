#include "core/externalCommands.hpp"
#include "config.hpp"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

int externalCommands(const std::vector<std::string> &tokens) {
  if (tokens.empty()) {
    return 0;
  }

  std::vector<char *> c_args;
  c_args.reserve(tokens.size() + 1);

  for (const auto &arg : tokens) {
    c_args.push_back(strdup(arg.c_str()));
  }
  c_args.push_back(nullptr);

  pid_t pid = fork();

  if (pid < 0) {
    perror("fork");
    for (char *ptr : c_args) {
      free(ptr);
    }
    return 1;
  }
  if (pid == 0) {
    if (execvp(c_args[0], c_args.data()) == -1) {
      std::cerr << shellName << "-> " << c_args[0] << ": command not found\n";
    }
    exit(EXIT_FAILURE);
  } else {
    int status;
    waitpid(pid, &status, 0);
    for (char *ptr : c_args) {
      free(ptr);
    }
    if (WIFEXITED(status)) {
      return WEXITSTATUS(status);
    }
  }

  return 0;
}
