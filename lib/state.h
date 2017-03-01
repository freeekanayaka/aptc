// Miscellaneous config-related APIs, mostly for setting config
// values, and checking/creating files.

#ifndef APTC_STATE_H_
#define APTC_STATE_H_

#include <apt-pkg/configuration.h>
#include <apt-pkg/cmndline.h>

// Initialize the given configuration with sane defaults.
void DefaultConfig(Configuration &Cnf);

// Initialize the data directory structure for the given configuration. The
// data directory is used by aptc to store files such as APT lists and archive
// caches that should not live in the container file system.
bool EnsureDataDirectories(Configuration &Cnf);

// Initialize the 'rootfs' configuration value and other paths relative to it.
void RootFsConfig(Configuration &Cnf);

// Check that the dpkg status file is present in the target rootfs.
bool CheckDpkgStatus(CommandLine &CmdL, Configuration &Cnf);

#endif // APTC_STATE_H_
