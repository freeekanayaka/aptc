#ifndef APTC_INIT_H_
#define APTC_INIT_H_

#include <apt-pkg/cmndline.h>
#include <apt-pkg/configuration.h>
#include <apt-pkg/fileutl.h>
#include <apt-pkg/error.h>

#include <fs.h>
#include <log.h>
#include <state.h>

// Execute the 'init' sub-command.
bool DoInit(CommandLine &CmdL) {

  auto DpkgStatus = _config->FindFile("Dir::State::status");

  if (RealFileExists(DpkgStatus))
    return _error->Error("Found existing dpkg status '%s'. Perhaps 'init' was already run?", DpkgStatus.c_str());

  // The suite is required
  if (CmdL.FileSize() == 1) {
    _error->Notice("Usage: aptc [options] init <suite> [<mirror>]");
    _error->Notice("Try `aptc init --help' for more information");
    return _error->Error("You must specify a suite");
  }

  auto Suite = CmdL.FileList[1];
  auto Mirror = "";

  if (CmdL.FileSize() > 2) {
    Mirror = CmdL.FileList[2];
  }

  Debug("Initializing suite '%s'", Suite);

  return (EnsureTrustedFile(*_config, Suite) &&
	  EnsureSourcesListFile(*_config, Suite, Mirror) &&
	  MakeFile(_config->FindFile("Dir::State::status")) &&
	  MakeFile(_config->FindFile("Dir::Etc::sourcelist")));
}

#endif // APTC_INIT_H_
