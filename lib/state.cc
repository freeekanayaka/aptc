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

  // Configuration, defaults to system settings.
  Cnf.Set("Dir::Etc::sourcelist", "sources.list");
  Cnf.Set("Dir::Etc::trusted", "trusted.gpg");
  Cnf.Set("Dir::Etc::trustedparts", "trusted.gpg.d");
  Cnf.Set("Dir::Bin::methods", "/usr/lib/apt/methods"); // Use the host methods

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

  // Tweaks specific to APTc
  Cnf.Set("Dir::Bin::dpkg", APTC_BINDIR "/dpkgc");
}

bool EnsureDataDirectories(Configuration &Cnf) {
  auto RootFs = Cnf.Find("rootfs");
  auto Paths = {
      Cnf.FindDir("Dir::Etc"),
      Cnf.FindDir("Dir::State"),
      Cnf.FindDir("Dir::State::lists"),
      Cnf.FindDir("Dir:Cache"),
      Cnf.FindDir("Dir::Cache::archives"),
      flCombine(RootFs, "var/log"),
      flCombine(RootFs, "var/lib/dpkg"),
      flCombine(RootFs, "var/lib/dpkg/updates"),
      flCombine(RootFs, "var/lib/dpkg/info"),
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

  // Conditionally set APT configuration files, so we don't overwrite user
  // preferences.
  Cnf.CndSet("Dir::Etc", flCombine(RootFs, "etc/apt/"));
  Cnf.CndSet("Dir::Etc::sourcelist","sources.list");
  Cnf.CndSet("Dir::Etc::sourceparts","sources.list.d/");
  Cnf.CndSet("Dir::Etc::main","apt.conf");
  Cnf.CndSet("Dir::Etc::parts","apt.conf.d");
  Cnf.CndSet("Dir::Etc::preferences","preferences");
  Cnf.CndSet("Dir::Etc::preferencesparts","preferences.d/");

  // Set dpkg options
  Cnf.Set("DPkg::Options::1", "--force-not-root");
  Cnf.Set("DPkg::Options::2", "--root=" + RootFs);
  Cnf.Set("DPkg::Options::3", "--force-script-chrootless");
  Cnf.Set("DPkg::Options::4", "--force-bad-path");
  Cnf.Set("DPkg::Options::5", "--log=" + flCombine(RootFs, "var/log/dpkg.log"));
}

bool CheckDpkgStatus(CommandLine &CmdL, Configuration &Cnf) {

  auto DpkgStatus = Cnf.FindFile("Dir::State::status");

  // If we're going to run the init command, we check that the dpkg status
  // file does *not* exist.
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

bool SuiteIsUbuntu(std::string Suite) {
  return (Suite == "xenial" ||
	  Suite == "yakkety" ||
	  Suite == "zesty" ||
	  Suite == "artful" ||
	  Suite == "bionic");
}

bool SuiteIsDebian(std::string Suite) {
  return (Suite == "stable" ||
	  Suite == "testing" ||
	  Suite == "unstable" ||
	  Suite == "sid");
}

bool EnsureTrustedFile(Configuration &Cnf, std::string Suite) {

  auto Target = Cnf.FindFile("Dir::Etc::trusted");

  if (FileExists(Target)) {
    return true;
  }

  std::string Source;

  if (SuiteIsUbuntu(Suite))
    Source = "/usr/share/keyrings/ubuntu-archive-keyring.gpg";
  else if (SuiteIsDebian(Suite))
    Source = "/usr/share/keyrings/debian-archive-keyring.gpg";
  else
    return _error->Error("Unknown suite '%s'", Suite.c_str());
  

  FileFd In;
  FileFd Out;

  Debug("Copying keyring file from '%s' to '%s'", Source.c_str(), Target.c_str());

  return (In.Open(Source, FileFd::ReadOnly) &&
	  Out.Open(Target, FileFd::WriteOnly | FileFd::Create) &&
	  CopyFile(In, Out) &&
	  In.Close() &&
	  Out.Close());
}

bool EnsureSourcesListFile(Configuration &Cnf, std::string Suite, std::string Mirror) {

  auto Target = Cnf.FindFile("Dir::Etc::sourcelist");

  if (FileExists(Target)) {
    return true;
  }

  Debug("Copying sources.list file '%s'", Target.c_str());

  FileFd Out;

  if (Mirror == "") {
    if (SuiteIsUbuntu(Suite))
      Mirror = "http://archive.ubuntu.com/ubuntu";
    else
      Mirror = "http://ftp.debian.org/debian";
  }
  std::string DebLine = "deb " + Mirror + " " + Suite + " main\n";

  return (Out.Open(Target, FileFd::WriteOnly | FileFd::Create) &&
	  Out.Write(DebLine.c_str(), DebLine.length()) &&
	  Out.Close());
}
