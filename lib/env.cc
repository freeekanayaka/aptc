// Environment variables helpers

#include <stdlib.h>

#include <cassert>
#include <string>

std::string SetEnv(std::string Key, std::string Value) {
  const char *PreviousValue = getenv(Key.c_str());
  assert(setenv(Key.c_str(), Value.c_str(), 1) == 0);
  return PreviousValue;
}
