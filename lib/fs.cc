// Filesystem utilities

#include <stdlib.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <pwd.h>
#include <libgen.h>

#include <vector>
#include <string>
#include <fstream>

#include <apt-pkg/error.h>
#include <apt-pkg/fileutl.h>

#include "log.h"

std::string CurrentWorkingDirectory() {
  char Path[MAXPATHLEN];
  getcwd(Path, MAXPATHLEN);
  return Path;
}

// Helper returning the user home directory.
std::string UserHome() {
  const char *Path;
  if ((Path = getenv("HOME")) == NULL) {
    Path = getpwuid(getuid())->pw_dir; // LCOV_EXCL_LINE (XXX use gmock to test this?)
  }
  return Path;
}

std::string UserAptcData() {
  return UserHome() + "/.local/share/aptc";
}

std::string MakeTemporaryDirectory() {
  char Template[] = "/tmp/tmpdir.XXXXXX";
  char *Path = mkdtemp(Template);
  return Path;

}

bool RemoveDirectory(std::string Path) {
  // XXX Consider adding a C++ implementation instead.
  auto Rm = "rm -rf " + Path;
  return system(Rm.c_str()) == 0;
}

bool MakeFile(std::string Path) {
  if (FileExists(Path)) {
    return true; // LCOV_EXCL_LINE
  }

  Debug("Creating empty file '%s'", Path.c_str());

  // Create the parent directory and its ancestors.
  std::string Parent = Path; // copy, since dirname() mutates it.
  Parent = dirname((char*)Parent.c_str());
  if (!CreateDirectory("/", Parent)) {
      return _error->Error("Failed to create directory '%s'", Parent.c_str()); // LCOV_EXCL_LINE
  }

  std::fstream Stream;

  Stream.open(Path, std::ios::out);
  if (Stream.fail()) {
    return _error->Error("Failed to create file '%s'", Path.c_str()); // LCOV_EXCL_LINE
  }
  Stream.close();

  return true;
}
