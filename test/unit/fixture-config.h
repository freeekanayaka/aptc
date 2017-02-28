#ifndef FIXTURE_CONFIG_H_
#define FIXTURE_CONFIG_H_

// Test fixture cleaning up the global config and error objects
// after each test.
class FixtureConfig {
 public:
  void SetUp();
  void TearDown();
};

#endif // FIXTURE_CONFIG_H_
