#include <apt-pkg/configuration.h>

#include "cmd.h"
#include "init.h"
#include "install.h"

int main(const int argc, const char *argv[]) {

  std::vector<CommandLine::Args> Args = {
    CommandLine::MakeArgs('r', "rootfs", "rootfs", 0),
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

  // Set the default configuration values before parsing the command line
  // arguments, since these take precedence.
  DefaultConfig(*_config);

  // Initialization hook.
  auto Init = [](CommandLine &){
    return InitDirectories(*_config);
  };

  CommandLineInfo Info = {Args, Cmds, HelpTemplate, Init};
  //!
  return RunCommandLine(Info, argc, argv);
}
