#ifndef FIXTURE_TEMPDIR_H_
#define FIXTURE_TEMPDIR_H_

#include <string>

// Test fixture creating temporary directory.
class FixtureTempDir {

 public:
  void SetUp();
  void TearDown();
  std::string Path;
};

#endif // FIXTURE_TEMPDIR_H_
