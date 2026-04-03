#include "built-ins/builtCd.hpp"
#include "core/shellContext.hpp"
#include <cstdlib>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

int builtCd(const std::vector<std::string> &tokens, ShellContext &ctx) {
  const char *path = nullptr;
  if (tokens.size() < 2) {
    path = getenv("HOME");
    if (!path) {
      std::cerr << "HOME is not defined\n";
      return 1;
    }
  } else {
    path = tokens[1].c_str();
  }
  if (chdir(path) != 0) {
    perror("cd");
    return 1;
  }
  return 0;
}
