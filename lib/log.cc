#include <iostream>
#include <string>

#include <apt-pkg/configuration.h>
#include <apt-pkg/error.h>

void Debug(const char *Description, ...) {

  if (!_config->FindB("Debug::aptc")) {
    return;
  }

  // XXX Re-use the APIs from apt-pkg/error.cc to not re-invent formatting.
  //     Perhaps we should move to some nice library.
  GlobalError Error;
  va_list args;
  size_t msgSize = 400;
  bool retry;
  do {
    va_start(args,Description);
    retry = Error.Insert(GlobalError::DEBUG, Description, args, msgSize);
    va_end(args);
  } while (retry);
  Error.DumpErrors(std::clog);
}
