#include "core/shell.hpp"
#include "core/dispatcher.hpp"
#include "core/shellContext.hpp"
#include "execution/pipelineCommands.hpp"
#include "parser/pipeParser.hpp"
#include "parser/tokenize.hpp"
#include "utils/hasPipe.hpp"
#include "utils/history.hpp"
#include "utils/historyGlobal.hpp"
#include "utils/printCwd.hpp"
#include <iostream>
#include <limits.h>
#include <string>
#include <unistd.h>

void shell(ShellContext &ctx) {
  history.load();

  std::string input{};

  while (ctx.running) {
    printCwd(ctx);

    if (!std::getline(std::cin, input)) {
      std::cout << "\n";
      break;
    }
    history.add(input);
    auto tokens = tokenize(input);

    if (hasPipe(tokens)) {
      std::vector<std::vector<std::string>> pipeCommands = pipeParser(tokens);
      if (!pipeCommands.empty() && pipeCommands.size() > 1) {
        ctx.lastExitStatus = pipelineCommands(pipeCommands);
      } else if (!pipeCommands.empty()) {
        ctx.lastExitStatus = dispatcher(pipeCommands[0], ctx);
      }
    } else {
      if (!tokens.empty()) {
        ctx.lastExitStatus = dispatcher(tokens, ctx);
      }
    }
  }
}
