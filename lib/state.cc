#include <apt-pkg/error.h>
#include <apt-pkg/fileutl.h>

#include <cstring>

#include "config.h"
#include "state.h"
#include "fs.h"
#include "log.h"

void DefaultConfig(Configuration &Cnf) {

  // XXX: There's a fair amount of duplication with apt-pkg/init.cc, here
  //      some reactoring in libapt itself would help reducing it.

  // General APT things
  Cnf.Set("APT::Architecture", COMMON_ARCH);
  Cnf.Set("APT::Install-Recommends", false);
  Cnf.Set("APT::Install-Suggests", false);
  Cnf.Set("Dir", CurrentWorkingDirectory());

  // State
  Cnf.Set("Dir::State", UserAptcData());
  Cnf.Set("Dir::State::lists", "lists/");

  // Cache
  Cnf.Set("Dir::Cache", UserAptcData());
  Cnf.Set("Dir::Cache::archives", "archives/");
  Cnf.Set("Dir::Cache::srcpkgcache", "srcpkgcache.bin");
  Cnf.Set("Dir::Cache::pkgcache", "pkgcache.bin");

  // State
  Cnf.Set("Dir::Log", UserAptcData());

  // Configuration
  Cnf.Set("Dir::Bin::methods", "/usr/lib/apt/methods");
  Cnf.CndSet("Dir::Etc::trusted", "/etc/apt/trusted.gpg");
  Cnf.CndSet("Dir::Etc::trustedparts","/etc/apt/trusted.gpg.d");

  Cnf.Set("Acquire::IndexTargets::deb::Packages::MetaKey", "$(COMPONENT)/binary-$(ARCHITECTURE)/Packages");
  Cnf.Set("Acquire::IndexTargets::deb::Packages::flatMetaKey", "Packages");
  Cnf.Set("Acquire::IndexTargets::deb::Packages::ShortDescription", "Packages");
  Cnf.Set("Acquire::IndexTargets::deb::Packages::Description", "$(RELEASE)/$(COMPONENT) $(ARCHITECTURE) Packages");
  Cnf.Set("Acquire::IndexTargets::deb::Packages::flatDescription", "$(RELEASE) Packages");
  Cnf.Set("Acquire::IndexTargets::deb::Packages::Optional", false);
  Cnf.Set("Acquire::IndexTargets::deb::Translations::MetaKey", "$(COMPONENT)/i18n/Translation-$(LANGUAGE)");
  Cnf.Set("Acquire::IndexTargets::deb::Translations::flatMetaKey", "$(LANGUAGE)");
  Cnf.Set("Acquire::IndexTargets::deb::Translations::ShortDescription", "Translation-$(LANGUAGE)");
  Cnf.Set("Acquire::IndexTargets::deb::Translations::Description", "$(RELEASE)/$(COMPONENT) Translation-$(LANGUAGE)");
  Cnf.Set("Acquire::IndexTargets::deb::Translations::flatDescription", "$(RELEASE) Translation-$(LANGUAGE)");
  Cnf.Set("Acquire::IndexTargets::deb-src::Sources::MetaKey", "$(COMPONENT)/source/Sources");
  Cnf.Set("Acquire::IndexTargets::deb-src::Sources::flatMetaKey", "Sources");
  Cnf.Set("Acquire::IndexTargets::deb-src::Sources::ShortDescription", "Sources");
  Cnf.Set("Acquire::IndexTargets::deb-src::Sources::Description", "$(RELEASE)/$(COMPONENT) Sources");
  Cnf.Set("Acquire::IndexTargets::deb-src::Sources::flatDescription", "$(RELEASE) Sources");
  Cnf.Set("Acquire::IndexTargets::deb-src::Sources::Optional", false);

  Cnf.Set("Acquire::Changelogs::URI::Origin::Debian", "http://metadata.ftp-master.debian.org/changelogs/@CHANGEPATH@_changelog");
  Cnf.Set("Acquire::Changelogs::URI::Origin::Tanglu", "http://metadata.tanglu.org/changelogs/@CHANGEPATH@_changelog");
  Cnf.Set("Acquire::Changelogs::URI::Origin::Ubuntu", "http://changelogs.ubuntu.com/changelogs/pool/@CHANGEPATH@/changelog");
  Cnf.Set("Acquire::Changelogs::URI::Origin::Ultimedia", "http://packages.ultimediaos.com/changelogs/pool/@CHANGEPATH@/changelog.txt");
  Cnf.Set("Acquire::Changelogs::AlwaysOnline::Origin::Ubuntu", true);
}

bool EnsureDataDirectories(Configuration &Cnf) {
  auto Paths = {
      Cnf.FindDir("Dir::State"),
      Cnf.FindDir("Dir::State::lists"),
      Cnf.FindDir("Dir:Cache"),
      Cnf.FindDir("Dir::Cache::archives"),
  };

  for (auto const& Path : Paths) {
    if(!DirectoryExists(Path)) {
      Debug("Create data directory '%s'", Path.c_str());
      if (!CreateDirectory("/", Path)) {
	return _error->Error("Failed to create directory '%s'", Path.c_str());
      }
    }
  }

  return true;
}

void RootFsConfig(Configuration &Cnf) {
  auto Cwd = CurrentWorkingDirectory();
  auto RootFs = Cnf.Find("rootfs");

  if (RootFs.empty()) {
    // If no --rootfs argument was given, default to the current working directory.
    RootFs = Cwd;
  } else if (!(RootFs[0] == '/')) {
    // If the given rootfs is not absolute, prepend the working directory.
    RootFs = flCombine(Cwd, RootFs);
  }

  Debug("Setting rootfs to '%s'", RootFs.c_str());
  Cnf.Set("rootfs", RootFs);

  // Set the dpkg status file relative to rootfs.
  Cnf.Set("Dir::State::status", flCombine(RootFs, "var/lib/dpkg/status"));

  Cnf.Set("Dir::Etc", flCombine(RootFs, "etc/apt/"));
  Cnf.Set("Dir::Etc::sourcelist","sources.list");
  Cnf.Set("Dir::Etc::sourceparts","sources.list.d");
  Cnf.Set("Dir::Etc::main","apt.conf");
  Cnf.Set("Dir::Etc::parts","apt.conf.d");
  Cnf.Set("Dir::Etc::preferences","preferences");
  Cnf.Set("Dir::Etc::preferencesparts","preferences.d");
}

bool CheckDpkgStatus(CommandLine &CmdL, Configuration &Cnf) {

  auto DpkgStatus = Cnf.FindFile("Dir::State::status");

  // If we're going to run the init command, don't bother to check if the
  // dpkg status file exists.
  if (strcmp(CmdL.FileList[0], "init") == 0) {
    return true;
  }

  if (!RealFileExists(DpkgStatus)) {
      return _error->Error("No dpkg status found: '%s'. Did you run 'init'?",
			   DpkgStatus.c_str());
  }
  Debug("Found dpkg status file at '%s'", DpkgStatus.c_str());

  return true;
}
