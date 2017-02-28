#include <stdlib.h>

#include "fs.h"
#include "env.h"

#include "fixture-home.h"

void FixtureHome::SetUp() {
  TempDir.SetUp();
  OriginalHome = SetEnv("HOME", TempDir.Path);
};

void FixtureHome::TearDown() {
  SetEnv("HOME", OriginalHome);
  TempDir.TearDown();
};
