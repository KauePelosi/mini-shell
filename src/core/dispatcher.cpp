#include "core/dispatcher.hpp"
#include "core/externalCommands.hpp"
#include "core/handlePipe.hpp"
#include "core/shellContext.hpp"
#include "utils/getInternMap.hpp"
#include "utils/hasPipe.hpp"
#include <string>
#include <unordered_map>
#include <vector>

int dispatcher(const std::vector<std::string> &tokens, ShellContext &ctx) {
  if (tokens.empty()) {
    return 0;
  }
  if (hasPipe(tokens)) {
    handlePipe(tokens, ctx);
  }

  const auto &InternMap = getInternMap();
  auto it = InternMap.find(tokens[0]);

  if (it != InternMap.end()) {
    return it->second(tokens, ctx);
  } else {
    return externalCommands(tokens);
  }
}
