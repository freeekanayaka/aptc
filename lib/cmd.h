#ifndef APTC_CMD_H_
#define APTC_CMD_H_

#include <string>

#include <apt-pkg/cmndline.h>

// A Dispatch object with additional help info.
struct DispatchWithHelp {
   const char *Match;
   bool (*Handler)(CommandLine &);
   const char *Help;
};

// Hold information about needed to build the CommandLine to run.
struct CommandLineInfo {
  std::vector<CommandLine::Args> Args;
  std::vector<DispatchWithHelp> Cmds;
  std::string HelpTemplate;
  bool (*Init)(CommandLine &);
};

// Run a command line with the given info and arguments.
int RunCommandLine(CommandLineInfo &Info, const int argc, const char *argv[]);

// Return a line with information about the binary version and architecture.
const std::string HelpVersion();

// Return help text for all available commands.
const std::string HelpCommands(std::vector<DispatchWithHelp> const &Cmds);

// Format a help template replacing the %VERSION% and %COMMAND% placeholder
// with the appropriate text.
const std::string HelpFormat(const std::string &Template,
			     std::vector<DispatchWithHelp> const &Cmds);

#endif // APTC_CMD_H_
