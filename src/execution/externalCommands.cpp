#include "execution/externalCommands.hpp"
#include "config.hpp"
#include "utils/hasRedirection.hpp"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <fcntl.h>
#include "signal.h"

std::vector<char *> getRedirectionArgs(const std::vector<std::string> &tokens) {
  std::vector<char *> cleanArgs;
  for (size_t i = 0; i < tokens.size(); i++) {
    if (tokens[i] == ">" || tokens[i] == ">>") {
      if (i + 1 >= tokens.size()) {
        std::cerr << shellName << "-> " << "syntax error: file doesn't specific\n";
        return {};
      }
      int flags = O_WRONLY | O_CREAT;
      if (tokens[i] == ">") {
        flags |= O_TRUNC;
      }
      else {
        flags |= O_APPEND;
      }
      int fd = open(tokens[i + 1].c_str(), flags, 0664);
      if (fd < 0) {
        std::cerr << shellName << "-> " << "error: was not possible open file.\n";
        return {};
      }
      if (dup2(fd, STDOUT_FILENO) < 0) {
        std::cerr << shellName << "-> " << "error: was not possible redirect output.\n";
        close(fd);
        return {};
      }
      close (fd);
      i++;
    }
    else {
      cleanArgs.push_back(const_cast<char *>(tokens[i].c_str()));
    }
  }
  cleanArgs.push_back(nullptr);
  return cleanArgs;
}

int externalCommands(const std::vector<std::string> &tokens) {
    if (tokens.empty()) return 0;

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
      signal(SIGINT, SIG_DFL);
        std::vector<char *> finalArgs;

        if (hasRedirection(tokens)) {
            finalArgs = getRedirectionArgs(tokens);
            if (finalArgs.empty()) exit(EXIT_FAILURE);
        } else {
            for (const auto &arg : tokens) {
                finalArgs.push_back(const_cast<char *>(arg.c_str()));
            }
            finalArgs.push_back(nullptr);
        }

        if (execvp(finalArgs[0], finalArgs.data()) == -1) {
            std::cerr << shellName << "-> " << finalArgs[0] << ": command not found\n";
            exit(EXIT_FAILURE);
        }
    } 
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }

    return 0;
}
