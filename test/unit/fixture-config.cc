#include <apt-pkg/configuration.h>
#include <apt-pkg/error.h>

#include "fixture-config.h"

void FixtureConfig::SetUp() {};

void FixtureConfig::TearDown() {
  _config->Clear();
  _error->Discard();
};
