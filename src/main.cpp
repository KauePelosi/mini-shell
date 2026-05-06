#include "core/shell.hpp"
#include "core/shellContext.hpp"
#include "signal.h"

int main() {
  ShellContext ctx;
  signal(SIGINT, SIG_IGN);
  shell(ctx);

  return 0;
}
