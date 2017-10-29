#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "config.h"

const char* PRELOAD = "LD_PRELOAD=" APTC_LIBDIR "/libaptc-dpkg.so";
const char* ROOTOPT = "--root";

int main(const int argc, const char *argv[]) {
  char *args[argc];
  char* preload = new char[strlen(PRELOAD)+1]();
  char* rootfs;
  int i, rc;

  memcpy(args, argv, sizeof args);
  args[argc] = NULL;

  for (i = 0; i < argc; i++) {
    if (strncmp(args[i], ROOTOPT, strlen(ROOTOPT)) == 0) {
      asprintf(&rootfs,"ROOTFS%s", strstr(args[i], "="));
      rc = putenv(rootfs);
      if (rc != 0) {
	return rc;
      }
    }
  }

  rc = putenv(strncpy(preload, PRELOAD, strlen(PRELOAD)+1));
  if (rc != 0) {
    return rc;
  }

  return execvpe("dpkg", args, environ);
}
