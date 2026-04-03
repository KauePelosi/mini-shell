#include "core/handlePipe.hpp"
#include "core/shellContext.hpp"
#include <cstddef>
#include <string>
#include <vector>

int handlePipe(const std::vector<std::string> &tokens, ShellContext &ctx) {
  size_t start = 0;
  for (size_t i = 0; i <= tokens.size(); i++) {
    if (i == tokens.size() || tokens[i] == "|") {
    }
  }
  return 0;
}
