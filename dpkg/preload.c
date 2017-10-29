#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

extern int execvpe(const char *file, char *const argv[], char *const envp[]);

// Wrapper around execvp that will no-op if the file being executed is
// a maintainer script.
int execvp(const char *file, char *const argv[]) {
  char* rootfs = getenv("ROOTFS");
  char* tmp = (char*) malloc(strlen(rootfs) + strlen("/var/lib/dpkg/tmp") + 1);
  char* info = (char*) malloc(strlen(rootfs) + strlen("/var/lib/dpkg/info") + 1);

  strcpy(tmp, rootfs);
  strcat(tmp, "/var/lib/dpkg/tmp");

  strcpy(info, rootfs);
  strcat(info, "/var/lib/dpkg/info");

  if (strncmp(tmp, file, strlen(tmp)) == 0) {
    exit(0);
  }

  if (strncmp(info, file, strlen(info)) == 0) {
    exit(0);
  }

  free(tmp);
  free(info);

  return execvpe(file, argv, __environ);
};

int chown(const char *pathname, uid_t owner, gid_t group) {
  return 0;
}

int fchown(int fd, uid_t owner, gid_t group) {
  return 0;
}

int lchown(const char *pathname, uid_t owner, gid_t group) {
  return 0;
}
