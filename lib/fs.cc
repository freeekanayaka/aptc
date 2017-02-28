// Filesystem utilities

#include <stdlib.h>
#include <unistd.h>
#include <sys/param.h>
#include <pwd.h>

#include <string>

// Trim trailing slash if present
std::string NoTrailingSlash(std::string Path) {
  Path.erase(Path.find_last_not_of('/') + 1);
  return Path;
}

std::string CurrentWorkingDirectory() {
  char Path[MAXPATHLEN];
  getcwd(Path, MAXPATHLEN);
  return NoTrailingSlash(Path);
}

// Helper returning the user home directory.
std::string UserHome() {
  const char *Path;
  if ((Path = getenv("HOME")) == NULL) {
    Path = getpwuid(getuid())->pw_dir; // LCOV_EXCL_LINE (XXX use gmock to test this?)
  }
  return NoTrailingSlash(Path);
}

std::string UserAptcData() {
  return UserHome() + "/.local/share/aptc";
}

std::string MakeTemporaryDirectory() {
  char Template[] = "/tmp/tmpdir.XXXXXX";
  char *Path = mkdtemp(Template);
  return Path;

}

bool MakeDirectories(std::string Path) {
  // XXX Consider adding a C++ implementation instead.
  auto Mkdir = "mkdir -p " + Path;
  return system(Mkdir.c_str()) == 0;
}

bool RemoveDirectory(std::string Path) {
  // XXX Consider adding a C++ implementation instead.
  auto Rm = "rm -rf " + Path;
  return system(Rm.c_str()) == 0;
}
