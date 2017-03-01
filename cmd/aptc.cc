#include <apt-pkg/configuration.h>
#include <apt-pkg/cmndline.h>
#include <apt-pkg/init.h>
#include <apt-pkg/pkgsystem.h>

#include "cmd.h"
#include "state.h"
#include "init.h"
#include "install.h"
#include "update.h"

int main(const int argc, const char *argv[]) {

  std::vector<CommandLine::Args> Args = {
    CommandLine::MakeArgs('r', "rootfs", "rootfs", CommandLine::HasArg),
  };

  std::vector<DispatchWithHelp> Cmds = {
    {"init", &DoInit, "Init"},
    {"install", &DoInstall, "Install"},
    {"update", &DoUpdate, "Update"},
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
  auto Init = [](CommandLine &CmdL){
    RootFsConfig(*_config);
    return pkgInitSystem(*_config, _system) &&
           EnsureDataDirectories(*_config) &&
           CheckDpkgStatus(CmdL, *_config);
  };

  CommandLineInfo Info = {Args, Cmds, HelpTemplate, Init};
  return RunCommandLine(Info, argc, argv);
}
