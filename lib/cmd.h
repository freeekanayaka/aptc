#ifndef APTC_CMD_H_
#define APTC_CMD_H_

#include <string>

#include <apt-pkg/cmndline.h>

struct DispatchWithHelp {
   const char *Match;
   bool (*Handler)(CommandLine &);
   const char *Help;
};

int RunCommandLine(std::vector<CommandLine::Args> &Args,
		   std::vector<DispatchWithHelp> &CmdsWithHelp,
		   std::string &HelpTemplate,
		   const int argc, const char *argv[]);

const std::string HelpVersion();
const std::string HelpCommands(std::vector<DispatchWithHelp> const &Cmds);
const std::string HelpFormat(const std::string &Template,
			     std::vector<DispatchWithHelp> const &Cmds);

#endif // APTC_CMD_H_
