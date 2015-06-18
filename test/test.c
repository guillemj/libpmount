#include "config.h"

#include <sys/stat.h>
#include <sys/types.h>

#include <errno.h>
#include <unistd.h>
#include <pmount.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define FSIMAGE FSNAME ".img"
#define FSMOUNT FSNAME ".mnt"

int
main()
{
  const char *env;
  int error;

  env = getenv("FAKEROOTKEY");
  if (env && strlen(env) != 0)
    return 77;

  if (geteuid() != 0)
    return 77;

  error = mkdir(FSMOUNT, 0755);
  if (error < 0 && errno != EEXIST)
  {
    perror("mkdir failed");
    return 1;
  }

  error = pmount(FSNAME, FSMOUNT, PMOUNT_READONLY, FSIMAGE);
  if (error == -1)
  {
    perror("pmount failed");
    return 1;
  }

  error = pumount(FSMOUNT, 0);
  if (error == -1)
  {
    perror("WARNING: pmount succeeded, but pumount failed");
    return 1;
  }

  return 0;
}
