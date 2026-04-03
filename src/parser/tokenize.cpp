#include "core/tokenize.hpp"
#include <cctype>
#include <string>
#include <vector>

void addToken(std::vector<std::string> &tokens, std::string &temp) {
  tokens.push_back(std::move(temp));
  temp.clear();
}

std::vector<std::string> tokenize(const std::string &input) {
  std::vector<std::string> tokens;
  std::string temp;

  bool inQuotes = false;
  char quoteChar = '\0';

  for (char c : input) {
    if (c == '"' || c == '\'') {
      if (!inQuotes) {
        inQuotes = true;
        quoteChar = c;
      } else if (quoteChar == c) {
        inQuotes = false;
      }
      continue;
    }
    if (!inQuotes && std::isspace(static_cast<unsigned char>(c))) {
      if (!temp.empty()) {
        addToken(tokens, temp);
      }
      continue;
    }
    temp += c;
  }
  if (!temp.empty()) {
    addToken(tokens, temp);
  }

  return tokens;
}
