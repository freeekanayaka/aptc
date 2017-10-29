#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <apt-pkg/error.h>
#include <apt-pkg/configuration.h>
#include <apt-pkg/cmndline.h>
#include <apt-pkg/fileutl.h>

#include "config.h"
#include "state.h"
#include "fs.h"

#include "fixture-home.h"
#include "fixture-tempdir.h"

// Appropriate defaults are set.
TEST(DefaultConfigTest, SetDefaults) {
  Configuration Cnf;
  DefaultConfig(Cnf);
  EXPECT_EQ(Cnf.Find("APT::Architecture"), COMMON_ARCH);
  EXPECT_FALSE(Cnf.FindB("APT::Install-Recommends"));
  EXPECT_FALSE(Cnf.FindB("APT::Install-Suggests"));
  EXPECT_THAT(Cnf.Find("Dir"), testing::StartsWith("/"));
}

// Any missing directory gets created.
TEST(EnsureDataDirectoriesTest, CreateMissingDirs) {

  FixtureHome Home;
  Home.SetUp();

  Configuration Cnf;
  DefaultConfig(Cnf);
  RootFsConfig(Cnf);

  EXPECT_TRUE(EnsureDataDirectories(Cnf));

  Home.TearDown();
}

// If the state directory can't be created, an error is set.
TEST(EnsureDataDirectoriesTest, DirStateError) {
  Configuration Cnf;
  Cnf.Set("Dir::State", "/dev/null");

  EXPECT_FALSE(EnsureDataDirectories(Cnf));
  EXPECT_TRUE(_error->PendingError());

  _error->Discard();
}

// If no --rootfs argument is passed, default to the current working directory.
TEST(RootFsConfigTest, NoRootFsArgument) {
  Configuration Cnf;

  RootFsConfig(Cnf);

  EXPECT_EQ(Cnf.Find("rootfs"), CurrentWorkingDirectory());
}

// If the --rootfs argument is passed, but it's a relative path, the current
// working directory gets prepended.
TEST(RootFsConfigTest, RelativeRootFs) {
  Configuration Cnf;
  Cnf.Set("rootfs", "foo/bar");

  RootFsConfig(Cnf);

  EXPECT_EQ(Cnf.FindDir("rootfs"), CurrentWorkingDirectory() + "/foo/bar/");
}

// If the --rootfs argument is passed and it's absolute, it's left unchanged.
TEST(RootFsConfigTest, AbsoluteRootFs) {
  Configuration Cnf;
  Cnf.Set("rootfs", "/foo/bar");

  RootFsConfig(Cnf);

  EXPECT_EQ(Cnf.FindDir("rootfs"), "/foo/bar/");
}

// If the dpkg status file is not there and we're not going to run the
// init sub-command, an error is returned.
TEST(CheckDpkgStatusTest, NoDpkgStatus) {
  Configuration Cnf;
  Cnf.Set("Dir::State::status", "/foo/bar");

  CommandLine CmdL;
  CmdL.FileList = new const char *[2];
  CmdL.FileList[0] = "install";
  CmdL.FileList[1] = 0;

  EXPECT_FALSE(CheckDpkgStatus(CmdL, Cnf));
  EXPECT_TRUE(_error->PendingError());

  _error->Discard();
}

// If the dpkg status file is not there, but we're not going to run the
// init sub-command, no error is returned.
TEST(CheckDpkgStatusTest, NoDpkgStatusInitCommand) {
  Configuration Cnf;
  Cnf.Set("Dir::State::status", "/foo/bar");

  CommandLine CmdL;
  CmdL.FileList = new const char *[2];
  CmdL.FileList[0] = "init";
  CmdL.FileList[1] = 0;

  EXPECT_TRUE(CheckDpkgStatus(CmdL, Cnf));
  EXPECT_FALSE(_error->PendingError());

  _error->Discard();
}

// If the dpkg status file there, no error is returned.
TEST(CheckDpkgStatusTest, DpkgStatusExists) {

  FixtureTempDir TempDir;
  TempDir.SetUp();

  Configuration Cnf;
  Cnf.Set("Dir::State::status", TempDir.Path + "/var/lib/dpkg/status");

  EXPECT_TRUE(MakeFile(Cnf.FindFile("Dir::State::status")));

  CommandLine CmdL;
  CmdL.FileList = new const char *[2];
  CmdL.FileList[0] = "update";
  CmdL.FileList[1] = 0;

  EXPECT_TRUE(CheckDpkgStatus(CmdL, Cnf));
  EXPECT_FALSE(_error->PendingError());

  _error->Discard();
}

// A deb line is written in sources.list
TEST(EnsureSourcesListFileTest, WriteDebLine) {
  FixtureTempDir TempDir;
  TempDir.SetUp();

  Configuration Cnf;
  Cnf.Set("Dir::Etc::sourcelist", TempDir.Path + "/sources.list");

  EXPECT_TRUE(EnsureSourcesListFile(Cnf, "sid", ""));
  EXPECT_TRUE(FileExists(TempDir.Path + "/sources.list"));
}
