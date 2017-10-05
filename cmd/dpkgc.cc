#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"

const char* PRELOAD = "LD_PRELOAD=" APTC_LIBDIR "/libaptc-dpkg.so";

int main(const int argc, const char *argv[]) {
  char *args[argc];
  char* preload = new char[strlen(PRELOAD)+1]();
  int rc;
  
  memcpy(args, argv, sizeof args);
  args[argc] = NULL;

  rc = putenv(strncpy(preload, PRELOAD, strlen(PRELOAD)+1));
  if (rc != 0) {
    return rc;
  }

  return execvpe("dpkg", args, environ);
}
