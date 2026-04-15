#include "parser/pipeParser.hpp"
#include <string>
#include <vector>

std::vector<std::vector<std::string>>
pipeParser(const std::vector<std::string> &tokens) {

  std::vector<std::vector<std::string>> commands;
  std::vector<std::string> current;

  for (const auto &token : tokens) {
    if (token == "|") {
      if (!current.empty()) {
        commands.push_back(current);
        current.clear();
      }
    } else {
      current.push_back(token);
    }
  }
  if (!current.empty()) {
    commands.push_back(current);
  }
  return commands;
}
