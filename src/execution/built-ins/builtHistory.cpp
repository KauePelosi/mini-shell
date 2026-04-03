#include "core/shellContext.hpp"
#include "utils/historyGlobal.hpp"
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

int builtHistory(const std::vector<std::string> &tokens, ShellContext &ctx) {
  const auto &entries = history.entries();
  for (size_t i = 0; i < entries.size(); ++i) {
    std::cout << i + 1 << " " << entries[i] << "\n";
  }
  return 0;
}
