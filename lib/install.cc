#include <iostream>

#include "install.h"

bool DoInstall(CommandLine &) {
  std::cout << "Install!" << std::endl;
  return true;
}
