#include "fs.h"

#include "gtest/gtest.h"

#include "fixture-tempdir.h"

void FixtureTempDir::SetUp() {
  Path = MakeTemporaryDirectory();
};

void FixtureTempDir::TearDown() {
  EXPECT_TRUE(RemoveDirectory(Path));
};
