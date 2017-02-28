#ifndef FIXTURE_HOME_H_
#define FIXTURE_HOME_H_

#include <string>

#include "fixture-tempdir.h"

// Test fixture setting $HOME to a temporary directory.
class FixtureHome {

 public:
  void SetUp();
  void TearDown();

 private:
  FixtureTempDir TempDir;
  std::string OriginalHome;
};

#endif // FIXTURE_HOME_H_
