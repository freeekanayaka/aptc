#ifndef APTC_FS_H_
#define APTC_FS_H_

#include <vector>
#include <string>

// Helper returning the current working directory.
std::string CurrentWorkingDirectory();

// Helper returning the user's default aptc data dir ($HOME/.local/share/aptc).
std::string UserAptcData();

// Create a temporary directory (the caller is supposed to remove it when appropriate).
std::string MakeTemporaryDirectory();

// Recursively remove the given directory, if it exists.
bool RemoveDirectory(std::string Path);

// Create an empty file at the given path. Intermediate directories will be
// created automatically.
bool MakeFile(std::string Path);

// Create the parent directory for the given path
bool CreateParentDirectory(std::string Path);

#endif // APTC_FS_H_
