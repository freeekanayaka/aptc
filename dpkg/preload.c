#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int endsWith(const char *str, const char *suffix)
{
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix >  lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

extern char **environ;
extern int execvpe(const char *file, char *const argv[], char *const envp[]);


int execvp(const char *file, char *const argv[]) {
  if (endsWith(file, "inst") > 0 || endsWith(file, "rm") > 0) {
    exit(0);
  }
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
