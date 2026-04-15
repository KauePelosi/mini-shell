#include "core/shellContext.hpp"
#include <iostream>
#include <string>
#include <vector>

int builtExit(const std::vector<std::string> &tokens, ShellContext &ctx) {
  int status = ctx.lastExitStatus;
  if (tokens.size() > 1) {
    try {
      status = std::stoi(tokens[1]);
    } catch (...) {
      std::cerr << "exit: numeric argument required\n";
    }
  }
  std::cout << "exit";
  ctx.lastExitStatus = status;
  ctx.running = false;
  return status;
}
