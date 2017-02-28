#ifndef APTC_ENV_H_
#define APTC_ENV_H_

#include <string>

// Helper setting the given environment variable and returning the existing
// value (if any).
std::string SetEnv(std::string Key, std::string Value);

#endif // APTC_ENV_H_
