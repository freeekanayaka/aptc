#include <config.h>

#include <iostream>
#include <cassert>
#include <cstring>
#include <string>
#include <regex>

#include <apt-pkg/configuration.h>
#include <apt-pkg/error.h>

#include "cmd.h"

// Run the given command line.
int RunCommandLine(std::vector<CommandLine::Args> &Args,
		   std::vector<DispatchWithHelp> &CmdsWithHelp,
		   std::string &HelpTemplate,
		   const int argc, const char *argv[]) {

  // Having no commands is not an option.
  assert(!CmdsWithHelp.empty() && CmdsWithHelp[0].Match != nullptr);

  // Automatically add the 'help' and 'version' arguments and terminate the list
  // with a "NULL" argument (this is expected by apt-pkg/contrib/cmndline.cc).
  Args.push_back(std::move(CommandLine::MakeArgs('h', "help", "help", 0)));
  Args.push_back(std::move(CommandLine::MakeArgs('v', "version", "version", 0)));
  Args.push_back(std::move(CommandLine::MakeArgs(0, NULL, NULL, 0)));

  CommandLine CmdL(Args.data(), _config);

  // If invalid arguments are given, print the help and fail.
  if (!CmdL.Parse(argc, argv)) {
    _error->DumpErrors();
    return 1;
  };

  // If '-h' or '--help' is passed, print the help and succeed.
  if (_config->FindB("help")) {
    std::cout << HelpFormat(HelpTemplate, CmdsWithHelp);
    return 0;
  };

  // If '-v' or '--version' is passed, print the version and succeed.
  if (_config->FindB("version")) {
    std::cout << HelpVersion();
    return 0;
  };

  // If no sub-command is given, print the help and fail.
  if (CmdL.FileSize() == 0) {
    std::cout << HelpFormat(HelpTemplate, CmdsWithHelp);
    return 1;
  }

  // If the given sub-command is 'help', print the help and succeed.
  if (strcmp(CmdL.FileList[0], "help") == 0) {
    std::cout << HelpFormat(HelpTemplate, CmdsWithHelp);
    return 0;
  }

  // Dispatch the sub-command.
  std::vector<CommandLine::Dispatch> Cmds;
  for (auto const& cmd : CmdsWithHelp) Cmds.push_back({cmd.Match, cmd.Handler});

  if (!CmdL.DispatchArg(Cmds.data())) {
    if (_error->PendingError()) {
      _error->DumpErrors();
    }
    return 100;
  };

   return 0;
}

// Return a line with information about the binary version and architecture.
const std::string HelpVersion() {
  return PACKAGE " " PACKAGE_VERSION " (" COMMON_ARCH ")\n";
}

// Return help text for all available commands.
const std::string HelpCommands(std::vector<DispatchWithHelp> const &Cmds) {

  std::string Help = "Available commands:\n";
  for (auto const &c: Cmds) {
    if (c.Help == nullptr) continue;
    Help += "  " + std::string(c.Match) + " - " + std::string(c.Help) + "\n";
  }

  return Help;
}

// Format a help template replacing the %VERSION% and %COMMAND% placeholder
// with the appropriate text.
const std::string HelpFormat(const std::string &Template,
			     std::vector<DispatchWithHelp> const &Cmds) {
  std::regex Version("%VERSION%");
  std::regex Commands("%COMMANDS%");

  std::string Help;
  Help = std::regex_replace(Template, Version, HelpVersion());
  Help = std::regex_replace(Help, Commands, HelpCommands(Cmds));

  return Help;
}
