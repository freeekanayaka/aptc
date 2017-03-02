#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <apt-pkg/configuration.h>

#include "log.h"

#include "fixture-config.h"

class DebugTest : public testing::Test {
protected:
  virtual void SetUp() {
    Config.SetUp();
  }
  virtual void TearDown() {
    Config.TearDown();
  }
  FixtureConfig Config;
};

// If Debug::aptc is not set, no output will be produced.
TEST(Debug, DebugFlagOff) {
  testing::internal::CaptureStderr();
  Debug("hello");
  EXPECT_EQ(testing::internal::GetCapturedStderr(), "");
}
// If Debug::aptc is set, messages will be dumped.
TEST(Debug, DebugFlagOn) {
  testing::internal::CaptureStderr();
  _config->Set("Debug::aptc", 1);
  Debug("hello");
  EXPECT_EQ(testing::internal::GetCapturedStderr(), "D: hello\n");
}
