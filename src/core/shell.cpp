#include "core/shell.hpp"
#include "core/dispatcher.hpp"
#include "core/shellContext.hpp"
#include "parser/tokenize.hpp"
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
    if (!tokens.empty()) {
      ctx.lastExitStatus = dispatcher(tokens, ctx);
    }
  }
}
