#include <apt-pkg/error.h>

#include "config.h"
#include "init.h"
#include "fs.h"

void DefaultConfig(Configuration &Cnf) {

  // XXX: There's a fair amount of duplication with apt-pkg/init.cc, here
  //      some reactoring in libapt itself would help reducing it.

  // General APT things
  Cnf.CndSet("APT::Architecture", COMMON_ARCH);
  Cnf.CndSet("APT::Install-Recommends", false);
  Cnf.CndSet("APT::Install-Suggests", false);
  Cnf.CndSet("Dir", CurrentWorkingDirectory());

  // State
  Cnf.CndSet("Dir::State", UserAptcData());
  Cnf.CndSet("Dir::State::lists", "lists/");

  // Cache
  Cnf.CndSet("Dir::Cache", UserAptcData());
  Cnf.CndSet("Dir::Cache::archives", "archives/");
  Cnf.CndSet("Dir::Cache::srcpkgcache", "srcpkgcache.bin");
  Cnf.CndSet("Dir::Cache::pkgcache", "pkgcache.bin");
}

bool InitDirectories(Configuration &Cnf) {
  auto Data = Cnf.Find("Dir::State");
  if (!MakeDirectories(Data)) {
    return _error->Error("Failed to create data directory '%s'", Data.c_str());
  };
  return true;
}
