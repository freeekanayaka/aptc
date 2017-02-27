#include "cmd.h"
#include "install.h"

int main(const int argc, const char *argv[]) {

  std::vector<CommandLine::Args> Args = {
    CommandLine::MakeArgs('h', "help", "help", 0),
    CommandLine::MakeArgs(0, NULL, NULL, 0),
  };

  std::vector<DispatchWithHelp> Cmds = {
    {"install", &DoInstall, "Install"},
    {nullptr, nullptr, nullptr},
  };

  std::string HelpTemplate =
    "%VERSION%"
    "Usage: aptc [options] command\n"
    "\n"
    "%COMMANDS%"
    "\n"
    "See aptc(8)\n";

  return RunCommandLine(Args, Cmds, HelpTemplate, argc, argv);
}
