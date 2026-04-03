#include "execution/hasPipe.hpp"
#include <string>
#include <vector>

bool hasPipe(const std::vector<std::string> &tokens) {
  for (auto &i : tokens) {
    if (i == "|") {
      return true;
    }
  }
  return false;
}
