#ifndef APTC_FS_H_
#define APTC_FS_H_

#include <string>

// Helper returning the current working directory.
std::string CurrentWorkingDirectory();

// Helper returning the user's default aptc data dir ($HOME/.local/share/aptc).
std::string UserAptcData();

// Create a temporary directory (the caller is supposed to remove it when appropriate).
std::string MakeTemporaryDirectory();

// Create all directories in the given path, if they not exists.
bool MakeDirectories(std::string Path);

// Recursively remove the given directory, if it exists.
bool RemoveDirectory(std::string Path);

#endif // APTC_FS_H_
