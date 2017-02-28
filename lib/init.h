#ifndef APTC_INIT_H_
#define APTC_INIT_H_

#include <apt-pkg/cmndline.h>

// Initialize the given configuration with sane defaults.
void DefaultConfig(Configuration &Cnf);

// Initialize the directory structure for the given configuration.
bool InitDirectories(Configuration &Cnf);

#endif // APTC_INIT_H_
