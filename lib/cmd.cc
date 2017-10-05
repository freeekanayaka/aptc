#include <config.h>

#include <iostream>
#include <cassert>
#include <cstring>
#include <string>
#include <regex>

#include <apt-pkg/configuration.h>
#include <apt-pkg/error.h>

#include "cmd.h"

int RunCommandLine(CommandLineInfo &Info, const int argc, const char *argv[]) {

  // Having no commands is not an option.
  assert(!Info.Cmds.empty() && Info.Cmds[0].Match != nullptr);

  // Automatically add the 'help' and 'version' arguments and terminate the list
  // with a "NULL" argument (this is expected by apt-pkg/contrib/cmndline.cc).
  Info.Args.push_back(std::move(CommandLine::MakeArgs('h', "help", "help", 0)));
  Info.Args.push_back(std::move(CommandLine::MakeArgs('v', "version", "version", 0)));
  Info.Args.push_back(std::move(CommandLine::MakeArgs(0, NULL, NULL, 0)));

  CommandLine CmdL(Info.Args.data(), _config);

  // If invalid arguments are given, print the help and fail.
  if (!CmdL.Parse(argc, argv)) {
    _error->DumpErrors();
    return 1;
  };

  // If '-h' or '--help' is passed, print the help and succeed.
  if (_config->FindB("help")) {
    std::cout << HelpFormat(Info.HelpTemplate, Info.Cmds);
    return 0;
  };

  // If '-v' or '--version' is passed, print the version and succeed.
  if (_config->FindB("version")) {
    std::cout << HelpVersion();
    return 0;
  };

  // If no sub-command is given, print the help and fail.
  if (CmdL.FileSize() == 0) {
    std::cout << HelpFormat(Info.HelpTemplate, Info.Cmds);
    return 1;
  }

  // If the given sub-command is 'help', print the help and succeed.
  if (strcmp(CmdL.FileList[0], "help") == 0) {
    std::cout << HelpFormat(Info.HelpTemplate, Info.Cmds);
    return 0;
  }

  // Run the given initialization hook.
  if (Info.Init == nullptr) {
    Info.Init = [](CommandLine &){return true;};
  }

  // Convert the DispatchWithHelp vector to a Dispatch one.
  std::vector<CommandLine::Dispatch> Cmds;
  for (auto const& cmd : Info.Cmds) Cmds.push_back({cmd.Match, cmd.Handler});

  // Run the init hook and dispatch the sub-command.
  if (!Info.Init(CmdL) || !CmdL.DispatchArg(Cmds.data())) {
    _error->DumpErrors(GlobalError::NOTICE);
    return 100;
  };

   return 0;
}

const std::string HelpVersion() {
  return PACKAGE " " PACKAGE_VERSION " (" COMMON_ARCH ")\n";
}

const std::string HelpCommands(std::vector<DispatchWithHelp> const &Cmds) {

  std::string Help = "Available commands:\n";
  for (auto const &c: Cmds) {
    if (c.Help == nullptr) continue;
    Help += "  " + std::string(c.Match) + " - " + std::string(c.Help) + "\n";
  }

  return Help;
}

const std::string HelpFormat(const std::string &Template,
			     std::vector<DispatchWithHelp> const &Cmds) {
  std::regex Version("%VERSION%");
  std::regex Commands("%COMMANDS%");

  std::string Help;
  Help = std::regex_replace(Template, Version, HelpVersion());
  Help = std::regex_replace(Help, Commands, HelpCommands(Cmds));

  return Help;
}
