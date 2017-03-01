#ifndef APTC_INIT_H_
#define APTC_INIT_H_

#include <apt-pkg/cmndline.h>
#include <apt-pkg/configuration.h>
#include <apt-pkg/fileutl.h>

#include <fs.h>

// Execute the 'init' sub-command.
bool DoInit(CommandLine &CmdL) {
  return MakeFile(_config->FindFile("Dir::State::status")) &&
         MakeFile(_config->FindFile("Dir::Etc::sourcelist"));
}

#endif // APTC_INIT_H_
