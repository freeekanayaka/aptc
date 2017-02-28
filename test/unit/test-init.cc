#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <apt-pkg/error.h>
#include <apt-pkg/configuration.h>

#include "config.h"
#include "init.h"

#include "fixture-home.h"

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
TEST(InitDirectoriesTest, CreateMissingDirs) {

  FixtureHome Home;
  Home.SetUp();

  Configuration Cnf;
  DefaultConfig(Cnf);

  EXPECT_TRUE(InitDirectories(Cnf));

  Home.TearDown();
}

// If the state directory can't be created, an error is set.
TEST(InitDirectoriesTest, DirStateError) {
  Configuration Cnf;
  Cnf.CndSet("Dir::State", "/not/possible");

  EXPECT_FALSE(InitDirectories(Cnf));
  EXPECT_TRUE(_error->PendingError());

  _error->Discard();
}
