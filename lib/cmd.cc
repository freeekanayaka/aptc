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

  CommandLine CmdL(Args.data(), _config);

  if (CmdL.Parse(argc, argv) == false) {
    _error->DumpErrors();
    return 1;
  };

  if (_config->FindB("help") == true) {
    std::cout << HelpFormat(HelpTemplate, CmdsWithHelp);
    return 0;
  };

  if (CmdL.FileSize() == 0) {
    std::cout << HelpFormat(HelpTemplate, CmdsWithHelp);
    return 1;
  }

  if (strcmp(CmdL.FileList[0], "help") == 0) {
    std::cout << HelpFormat(HelpTemplate, CmdsWithHelp);
    return 0;
  }

   std::vector<CommandLine::Dispatch> Cmds;
   for (auto const& cmd : CmdsWithHelp) Cmds.push_back({cmd.Match, cmd.Handler});

  if (CmdL.DispatchArg(Cmds.data()) == false) {
    if (_error->PendingError() == true) {
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
